// ============================================================================
// 文件功能：Excel 病例数据解析器声明。基于 QXlsx 读取 .xlsx，按固定列模板
//           解析病例数据并逐行校验，返回每行的解析结果与错误信息。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/PatientCase.h"

// 单行解析结果
struct ParsedCaseRow
{
    int rowNumber = 0;   // Excel 行号（表头为第 1 行，数据从第 2 行起）
    PatientCase data;    // 解析出的病例数据
    QString error;       // 空 = 校验通过；非空 = 错误原因
};

// Excel 解析整体结果
struct ExcelParseResult
{
    bool ok = false;     // 文件能否打开并解析
    QString error;       // 整体错误信息
    QVector<ParsedCaseRow> rows;

    int validCount() const;    // 校验通过的行数
    int invalidCount() const;  // 校验失败的行数
};

class ExcelCaseParser
{
public:
    // 解析 .xlsx 病例文件。表头列依次为：
    // 病例号|姓名|性别|年龄|科室|诊断|入院日期|出院日期
    static ExcelParseResult parse(const QString &filePath);
};
