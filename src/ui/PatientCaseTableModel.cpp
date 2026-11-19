// ============================================================================
// 文件功能：病例表格模型实现，展示病例号/姓名/性别/年龄/科室/诊断/入院/出院。
// ============================================================================
#include "PatientCaseTableModel.h"

PatientCaseTableModel::PatientCaseTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PatientCaseTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : cases_.size();
}

int PatientCaseTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 8;
}

QVariant PatientCaseTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= cases_.size())
        return QVariant();
    if (role != Qt::DisplayRole && role != Qt::UserRole)
        return QVariant();

    const PatientCase &c = cases_.at(index.row());
    switch (index.column()) {
    case 0: return c.caseNo;
    case 1: return c.patientName;
    case 2: return c.gender;
    case 3: return c.age >= 0 ? QVariant(c.age) : QVariant();
    case 4: return c.department;
    case 5: return c.diagnosis;
    case 6: return c.admitDate.isValid() ? c.admitDate.toString(QStringLiteral("yyyy-MM-dd")) : QString();
    case 7: return c.dischargeDate.isValid() ? c.dischargeDate.toString(QStringLiteral("yyyy-MM-dd")) : QString();
    default: return QVariant();
    }
}

QVariant PatientCaseTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractTableModel::headerData(section, orientation, role);

    static const char *names[] = {"病例号", "姓名", "性别", "年龄", "科室", "诊断", "入院日期", "出院日期"};
    if (section < 0 || section >= 8)
        return QVariant();
    return QString::fromUtf8(names[section]);
}

void PatientCaseTableModel::setCases(const QVector<PatientCase> &cases)
{
    beginResetModel();
    cases_ = cases;
    endResetModel();
}

PatientCase PatientCaseTableModel::caseAt(int row) const
{
    if (row < 0 || row >= cases_.size())
        return PatientCase();
    return cases_.at(row);
}
