// ============================================================================
// 文件功能：mes_equipment_test 表的数据访问对象（DAO）声明，维护设备测试数据。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/EquipmentTest.h"

class EquipmentTestDao
{
public:
    bool insert(const EquipmentTest &t, quint32 *newId = nullptr, QString *error = nullptr);
    // keyword 模糊匹配：设备名称/型号/测试项/测试人
    QVector<EquipmentTest> findByProject(quint32 projectId, const QString &keyword = QString(),
                                         QString *error = nullptr);
    bool update(const EquipmentTest &t, QString *error = nullptr);
    bool remove(quint32 id, QString *error = nullptr);
    void removeByProject(quint32 projectId);
};
