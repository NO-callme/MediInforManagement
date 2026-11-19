// ============================================================================
// 文件功能：导入批次实体 ImportRecord 的定义，对应数据库表 mes_import_record，
//           用于追溯每次 Excel 导入的源文件与成功/失败统计。
// ============================================================================
#pragma once

#include <QDateTime>
#include <QString>

struct ImportRecord
{
    quint32 id = 0;
    QString fileName;      // 源文件名
    int totalRows = 0;     // 总行数
    int successRows = 0;   // 成功行数
    int failedRows = 0;    // 失败行数
    QDateTime importTime;  // 导入时间
    QString operatorName;  // 操作人（对应库字段 operator）
};
