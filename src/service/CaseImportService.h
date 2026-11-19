// ============================================================================
// 文件功能：病例导入服务声明。编排「解析 → 校验 → 事务批量入库 → 写导入记录」
//           的完整导入流程。
// ============================================================================
#pragma once

#include <QString>

#include "excel/ExcelCaseParser.h"

class CaseImportService
{
public:
    struct ImportSummary
    {
        int total = 0;          // 总数据行数
        int success = 0;        // 成功导入行数
        int failed = 0;         // 失败行数
        quint32 importRecordId = 0; // 生成的导入批次 id
    };

    // 校验并导入：事务批量写库 + 写导入记录
    static ImportSummary importFile(const QString &filePath, const QString &operatorName,
                                    QString *error = nullptr);

    // 仅校验：只解析返回结果，不写库
    static ExcelParseResult validateFile(const QString &filePath);
};
