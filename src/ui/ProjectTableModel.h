// ============================================================================
// 文件功能：项目表格模型 ProjectTableModel 声明，基于 QAbstractTableModel，
//           将 QVector<Project> 展示到 QTableView。
// ============================================================================
#pragma once

#include <QAbstractTableModel>
#include <QVector>

#include "model/Project.h"

class ProjectTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ProjectTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void setProjects(const QVector<Project> &projects);
    Project projectAt(int row) const;

private:
    QVector<Project> projects_;
};
