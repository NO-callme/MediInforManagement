// ============================================================================
// 文件功能：设备台账表格模型实现，展示编号/名称/型号/分类/状态/采购日期/科室。
// ============================================================================
#include "EquipmentTableModel.h"

EquipmentTableModel::EquipmentTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int EquipmentTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : list_.size();
}

int EquipmentTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 7;
}

QVariant EquipmentTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= list_.size())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();

    const Equipment &e = list_.at(index.row());
    switch (index.column()) {
    case 0: return e.equipmentNo;
    case 1: return e.name;
    case 2: return e.model;
    case 3: return e.category;
    case 4: return e.status;
    case 5: return e.purchaseDate.isValid() ? e.purchaseDate.toString(QStringLiteral("yyyy-MM-dd")) : QString();
    case 6: return e.department;
    default: return QVariant();
    }
}

QVariant EquipmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractTableModel::headerData(section, orientation, role);

    static const char *names[] = {"设备编号", "名称", "型号", "分类", "状态", "采购日期", "科室"};
    if (section < 0 || section >= 7)
        return QVariant();
    return QString::fromUtf8(names[section]);
}

void EquipmentTableModel::setEquipments(const QVector<Equipment> &list)
{
    beginResetModel();
    list_ = list;
    endResetModel();
}

Equipment EquipmentTableModel::equipmentAt(int row) const
{
    if (row < 0 || row >= list_.size())
        return Equipment();
    return list_.at(row);
}
