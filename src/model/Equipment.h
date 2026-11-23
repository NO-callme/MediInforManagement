// ============================================================================
// 文件功能：设备台账实体 Equipment 与过滤条件 EquipmentFilter 的定义，
//           对应数据库表 mes_equipment（二次开发新增模块）。
// ============================================================================
#pragma once

#include <QDate>
#include <QDateTime>
#include <QString>

struct Equipment
{
    quint32 id = 0;
    QString equipmentNo;   // 设备编号（必填，唯一）
    QString name;          // 设备名称
    QString model;         // 型号
    QString category;      // 分类
    QString status;        // 在用 / 维修 / 报废
    QDate purchaseDate;    // 采购日期
    QString department;    // 所属科室
    QString remark;        // 备注
    QDateTime createTime;
};

struct EquipmentFilter
{
    QString keyword;   // 模糊匹配：编号/名称/型号/科室
    QString status;    // 精确状态（空=全部）
    QString category;  // 精确分类（空=全部）
};
