// ============================================================================
// 文件功能：病例导入服务实现。解析后过滤重复/非法行，校验通过的行在事务中
//           批量入库，并写入 mes_import_record 导入批次记录。
// ============================================================================
#include "CaseImportService.h"

#include <QFileInfo>
#include <QSet>
#include <QStringList>

#include "common/DatabaseManager.h"
#include "dao/ImportRecordDao.h"
#include "dao/PatientCaseDao.h"

CaseImportService::ImportSummary CaseImportService::importFile(const QString &filePath,
                                                               const QString &operatorName,
                                                               QString *error)
{
    ImportSummary summary;

    const ExcelParseResult parsed = ExcelCaseParser::parse(filePath);
    if (!parsed.ok) {
        if (error)
            *error = parsed.error;
        return summary;
    }

    summary.total = parsed.rows.size();

    // 分类：校验通过且病例号不重复 -> 待插入；否则计为失败
    PatientCaseDao caseDao;
    QVector<PatientCase> toInsert;

    // 先收集校验通过的行，再一次性批量查重（避免逐行查询）
    QVector<PatientCase> validRows;
    QStringList caseNos;
    for (const ParsedCaseRow &row : parsed.rows) {
        if (!row.error.isEmpty()) {
            ++summary.failed;
            continue;
        }
        validRows.append(row.data);
        caseNos << row.data.caseNo;
    }

    QString dupErr;
    const QSet<QString> existing = caseDao.findExistingCaseNos(caseNos, &dupErr);
    if (!dupErr.isEmpty()) {
        if (error)
            *error = dupErr;
        return summary;
    }
    for (const PatientCase &c : validRows) {
        if (existing.contains(c.caseNo)) {
            ++summary.failed; // 病例号已存在
            continue;
        }
        toInsert.append(c);
    }

    DatabaseManager &db = DatabaseManager::instance();
    if (!db.beginTransaction()) {
        if (error)
            *error = QStringLiteral("开启事务失败");
        return summary;
    }

    // 写导入记录
    ImportRecord record;
    record.fileName = QFileInfo(filePath).fileName();
    record.totalRows = summary.total;
    record.successRows = toInsert.size();
    record.failedRows = summary.total - toInsert.size();
    record.operatorName = operatorName;

    ImportRecordDao recordDao;
    quint32 recordId = 0;
    if (!recordDao.insert(record, &recordId, error)) {
        db.rollback();
        return summary;
    }
    summary.importRecordId = recordId;

    // 批量写入病例
    for (const PatientCase &c : toInsert) {
        PatientCase toWrite = c;
        toWrite.importRecordId = recordId;
        QString err;
        if (!caseDao.insert(toWrite, nullptr, &err)) {
            db.rollback();
            if (error)
                *error = QStringLiteral("写入病例失败（病例号 %1）：%2").arg(c.caseNo, err);
            summary.success = 0;
            return summary;
        }
    }

    if (!db.commit()) {
        db.rollback();
        if (error)
            *error = QStringLiteral("提交事务失败");
        return summary;
    }

    summary.success = toInsert.size();
    summary.failed = summary.total - summary.success;
    return summary;
}

ExcelParseResult CaseImportService::validateFile(const QString &filePath)
{
    return ExcelCaseParser::parse(filePath);
}
