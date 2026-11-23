// ============================================================================
// 文件功能：设备台账表格模型 EquipmentTableModel 声明，基于 QAbstractTableModel，
//           将 QVector<Equipment> 展示到 QTableView。
// ============================================================================
#pragma once

#include <QAbstractTableModel>
#include <QVector>

#include "model/Equipment.h"

class EquipmentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EquipmentTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void setEquipments(const QVector<Equipment> &list);
    Equipment equipmentAt(int row) const;

private:
    QVector<Equipment> list_;
};
