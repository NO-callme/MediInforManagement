// ============================================================================
// 文件功能：导入自测程序。验证 Excel 解析 + 事务批量入库 + 导入记录全链路。
//           用法：import_selftest [config.ini路径] [xlsx路径]
// ============================================================================
#include <QCoreApplication>
#include <QDebug>

#include "common/DatabaseManager.h"
#include "dao/ImportRecordDao.h"
#include "dao/PatientCaseDao.h"
#include "service/CaseImportService.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString configPath = QStringLiteral("config.ini");
    QString xlsxPath = QStringLiteral("resources/病例数据.xlsx");
    if (argc > 1)
        configPath = QString::fromLocal8Bit(argv[1]);
    if (argc > 2)
        xlsxPath = QString::fromLocal8Bit(argv[2]);

    DatabaseManager &db = DatabaseManager::instance();
    if (!db.loadConfig(configPath)) {
        qCritical() << "加载配置失败:" << configPath;
        return 1;
    }
    QString err;
    if (!db.connect(&err)) {
        qCritical() << "连接失败:" << err;
        return 1;
    }

    // 清空历史数据，保证可重复测试
    db.execute(QStringLiteral("DELETE FROM mes_patient_case"));
    db.execute(QStringLiteral("DELETE FROM mes_import_record"));

    const CaseImportService::ImportSummary s = CaseImportService::importFile(xlsxPath, QStringLiteral("admin"), &err);
    if (!err.isEmpty())
        qWarning() << "导入错误:" << err;
    qInfo() << "导入结果: total=" << s.total << " success=" << s.success
            << " failed=" << s.failed << " recordId=" << s.importRecordId;

    PatientCaseDao caseDao;
    PatientCaseFilter filter;
    qInfo() << "病例表总数:" << caseDao.count(filter, &err);

    ImportRecordDao recordDao;
    qInfo() << "导入记录总数:" << recordDao.count(&err);

    // 抽样查前 3 条验证入库内容
    const QVector<PatientCase> sample = caseDao.find(filter, 3, 0, &err);
    for (const PatientCase &c : sample)
        qInfo() << "  " << c.id << c.caseNo << c.patientName << c.department << c.diagnosis;

    qInfo() << "==== 导入自测结束 ====";
    return (s.success == 20) ? 0 : 2;
}
