// ============================================================================
// 文件功能：病例表格模型 PatientCaseTableModel 声明，基于 QAbstractTableModel，
//           将 QVector<PatientCase> 以只读方式展示到 QTableView。
// ============================================================================
#pragma once

#include <QAbstractTableModel>
#include <QVector>

#include "model/PatientCase.h"

class PatientCaseTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PatientCaseTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void setCases(const QVector<PatientCase> &cases);
    PatientCase caseAt(int row) const;

private:
    QVector<PatientCase> cases_;
};
