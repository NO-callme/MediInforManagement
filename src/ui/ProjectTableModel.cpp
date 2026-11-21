// ============================================================================
// 文件功能：项目表格模型实现，展示编号/名称/状态/负责人/进度/起止日期。
// ============================================================================
#include "ProjectTableModel.h"

ProjectTableModel::ProjectTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int ProjectTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : projects_.size();
}

int ProjectTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 7;
}

QVariant ProjectTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= projects_.size())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();

    const Project &p = projects_.at(index.row());
    switch (index.column()) {
    case 0: return p.projectNo;
    case 1: return p.name;
    case 2: return p.status;
    case 3: return p.owner;
    case 4: return QString::number(p.progress) + QStringLiteral("%");
    case 5: return p.startDate.isValid() ? p.startDate.toString(QStringLiteral("yyyy-MM-dd")) : QString();
    case 6: return p.endDate.isValid() ? p.endDate.toString(QStringLiteral("yyyy-MM-dd")) : QString();
    default: return QVariant();
    }
}

QVariant ProjectTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractTableModel::headerData(section, orientation, role);

    static const char *names[] = {"项目编号", "名称", "状态", "负责人", "进度", "开始日期", "结束日期"};
    if (section < 0 || section >= 7)
        return QVariant();
    return QString::fromUtf8(names[section]);
}

void ProjectTableModel::setProjects(const QVector<Project> &projects)
{
    beginResetModel();
    projects_ = projects;
    endResetModel();
}

Project ProjectTableModel::projectAt(int row) const
{
    if (row < 0 || row >= projects_.size())
        return Project();
    return projects_.at(row);
}
