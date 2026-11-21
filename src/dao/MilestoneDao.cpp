// ============================================================================
// 文件功能：mes_project_milestone 表数据访问对象的实现。
// ============================================================================
#include "MilestoneDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns = "id, project_id, name, plan_date, actual_date, status";

QString sqlDate(const QDate &d)
{
    return d.isValid() ? QStringLiteral("'%1'").arg(d.toString(QStringLiteral("yyyy-MM-dd")))
                       : QStringLiteral("NULL");
}

Milestone rowToMilestone(const DbResult &rs)
{
    Milestone m;
    m.id = rs.value(0).toUInt();
    m.projectId = rs.value(1).toUInt();
    m.name = rs.value(2);
    m.planDate = QDate::fromString(rs.value(3), QStringLiteral("yyyy-MM-dd"));
    m.actualDate = QDate::fromString(rs.value(4), QStringLiteral("yyyy-MM-dd"));
    m.status = rs.value(5);
    return m;
}

} // namespace

bool MilestoneDao::insert(const Milestone &m, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_project_milestone (project_id, name, plan_date, actual_date, status) "
                            "VALUES (%1, '%2', %3, %4, '%5')")
                            .arg(m.projectId)
                            .arg(db.escape(m.name), sqlDate(m.planDate), sqlDate(m.actualDate),
                                 db.escape(m.status));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

QVector<Milestone> MilestoneDao::findByProject(quint32 projectId, QString *error)
{
    QVector<Milestone> list;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_project_milestone WHERE project_id = %2 ORDER BY plan_date, id")
                            .arg(kColumns)
                            .arg(projectId);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return list;
    while (rs.next())
        list.append(rowToMilestone(rs));
    return list;
}

bool MilestoneDao::update(const Milestone &m, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "UPDATE mes_project_milestone SET name='%1', plan_date=%2, actual_date=%3, status='%4' "
                            "WHERE id=%5")
                            .arg(db.escape(m.name),
                                 sqlDate(m.planDate),
                                 sqlDate(m.actualDate),
                                 db.escape(m.status),
                                 QString::number(m.id));
    return db.execute(sql, nullptr, error);
}

bool MilestoneDao::remove(quint32 id, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    return db.execute(QStringLiteral("DELETE FROM mes_project_milestone WHERE id = %1").arg(id), nullptr, error);
}

void MilestoneDao::removeByProject(quint32 projectId)
{
    DatabaseManager &db = DatabaseManager::instance();
    db.execute(QStringLiteral("DELETE FROM mes_project_milestone WHERE project_id = %1").arg(projectId));
}
