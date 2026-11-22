// ============================================================================
// 文件功能：mes_ocr_record 表的数据访问对象（DAO）声明，用于保存/查询识别记录。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/OcrRecord.h"

class OcrRecordDao
{
public:
    bool insert(const OcrRecord &r, quint32 *newId = nullptr, QString *error = nullptr);
    QVector<OcrRecord> findAll(int limit = 100, int offset = 0, QString *error = nullptr);
    int count(QString *error = nullptr);
};
