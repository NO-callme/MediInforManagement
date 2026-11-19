// ============================================================================
// 文件功能：mes_import_record 表的数据访问对象（DAO）声明，用于记录和查询
//           每次 Excel 导入批次。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/ImportRecord.h"

class ImportRecordDao
{
public:
    bool insert(ImportRecord &r, quint32 *newId = nullptr, QString *error = nullptr);
    bool findById(quint32 id, ImportRecord *out = nullptr, QString *error = nullptr);
    QVector<ImportRecord> findAll(int limit = 100, int offset = 0, QString *error = nullptr);
    int count(QString *error = nullptr);
};
