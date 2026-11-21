// ============================================================================
// 文件功能：设备测试实体 EquipmentTest 的定义，对应数据库表 mes_equipment_test。
// ============================================================================
#pragma once

#include <QDate>
#include <QString>

struct EquipmentTest
{
    quint32 id = 0;
    quint32 projectId = 0;   // 所属项目
    QString equipmentName;   // 设备名称
    QString model;           // 型号
    QString testItem;        // 测试项
    QString testResult;      // 测试结果
    QDate testDate;          // 测试日期
    QString tester;          // 测试人
};
