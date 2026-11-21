// ============================================================================
// 文件功能：mes_project_member 表数据访问对象的实现。
// ============================================================================
#include "ProjectMemberDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns = "id, project_id, name, role, phone, remark";

QString sqlStr(DatabaseManager &db, const QString &s)
{
    return s.isEmpty() ? QStringLiteral("NULL")
                       : QStringLiteral("'%1'").arg(db.escape(s));
}

ProjectMember rowToMember(const DbResult &rs)
{
    ProjectMember m;
    m.id = rs.value(0).toUInt();
    m.projectId = rs.value(1).toUInt();
    m.name = rs.value(2);
    m.role = rs.value(3);
    m.phone = rs.value(4);
    m.remark = rs.value(5);
    return m;
}

} // namespace

bool ProjectMemberDao::insert(const ProjectMember &m, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_project_member (project_id, name, role, phone, remark) "
                            "VALUES (%1, '%2', %3, %4, %5)")
                            .arg(m.projectId)
                            .arg(db.escape(m.name), sqlStr(db, m.role), sqlStr(db, m.phone), sqlStr(db, m.remark));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

QVector<ProjectMember> ProjectMemberDao::findByProject(quint32 projectId, QString *error)
{
    QVector<ProjectMember> list;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_project_member WHERE project_id = %2 ORDER BY id")
                            .arg(kColumns)
                            .arg(projectId);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return list;
    while (rs.next())
        list.append(rowToMember(rs));
    return list;
}

bool ProjectMemberDao::update(const ProjectMember &m, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "UPDATE mes_project_member SET name='%1', role=%2, phone=%3, remark=%4 WHERE id=%5")
                            .arg(db.escape(m.name),
                                 sqlStr(db, m.role),
                                 sqlStr(db, m.phone),
                                 sqlStr(db, m.remark),
                                 QString::number(m.id));
    return db.execute(sql, nullptr, error);
}

bool ProjectMemberDao::remove(quint32 id, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    return db.execute(QStringLiteral("DELETE FROM mes_project_member WHERE id = %1").arg(id), nullptr, error);
}

void ProjectMemberDao::removeByProject(quint32 projectId)
{
    DatabaseManager &db = DatabaseManager::instance();
    db.execute(QStringLiteral("DELETE FROM mes_project_member WHERE project_id = %1").arg(projectId));
}
