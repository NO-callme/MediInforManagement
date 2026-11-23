// ============================================================================
// 文件功能：mes_equipment 表的数据访问对象（DAO）声明，提供设备台账的
//           CRUD、条件筛选与分页查询。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/Equipment.h"

class EquipmentDao
{
public:
    bool insert(const Equipment &e, quint32 *newId = nullptr, QString *error = nullptr);
    bool findById(quint32 id, Equipment *out = nullptr, QString *error = nullptr);
    QVector<Equipment> find(const EquipmentFilter &filter, int limit = 100, int offset = 0,
                            QString *error = nullptr);
    int count(const EquipmentFilter &filter, QString *error = nullptr);
    bool update(const Equipment &e, QString *error = nullptr);
    bool remove(quint32 id, QString *error = nullptr);
    int countByNo(const QString &equipmentNo, QString *error = nullptr); // 查重
};
