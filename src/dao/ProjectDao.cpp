// ============================================================================
// 文件功能：mes_project 表数据访问对象的实现。
// ============================================================================
#include "ProjectDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns =
    "id, project_no, name, status, owner, start_date, end_date, progress, description";

QString sqlStr(DatabaseManager &db, const QString &s)
{
    return s.isEmpty() ? QStringLiteral("NULL")
                       : QStringLiteral("'%1'").arg(db.escape(s));
}

QString sqlDate(const QDate &d)
{
    return d.isValid() ? QStringLiteral("'%1'").arg(d.toString(QStringLiteral("yyyy-MM-dd")))
                       : QStringLiteral("NULL");
}

QString buildWhere(const ProjectFilter &f)
{
    DatabaseManager &db = DatabaseManager::instance();
    QStringList conds;
    if (!f.keyword.isEmpty()) {
        const QString kw = db.escape(f.keyword);
        conds << QStringLiteral(
                     "(project_no LIKE '%%1%' OR name LIKE '%%1%' OR owner LIKE '%%1%')")
                     .arg(kw);
    }
    if (!f.status.isEmpty())
        conds << QStringLiteral("status = '%1'").arg(db.escape(f.status));
    return conds.isEmpty() ? QString()
                           : QStringLiteral(" WHERE ") + conds.join(QStringLiteral(" AND "));
}

Project rowToProject(const DbResult &rs)
{
    Project p;
    p.id = rs.value(0).toUInt();
    p.projectNo = rs.value(1);
    p.name = rs.value(2);
    p.status = rs.value(3);
    p.owner = rs.value(4);
    p.startDate = QDate::fromString(rs.value(5), QStringLiteral("yyyy-MM-dd"));
    p.endDate = QDate::fromString(rs.value(6), QStringLiteral("yyyy-MM-dd"));
    p.progress = rs.value(7).toInt();
    p.description = rs.value(8);
    return p;
}

} // namespace

bool ProjectDao::insert(const Project &p, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_project "
                            "(project_no, name, status, owner, start_date, end_date, progress, description) "
                            "VALUES ('%1', %2, '%3', %4, %5, %6, %7, %8)")
                            .arg(db.escape(p.projectNo),
                                 sqlStr(db, p.name),
                                 db.escape(p.status),
                                 sqlStr(db, p.owner),
                                 sqlDate(p.startDate),
                                 sqlDate(p.endDate),
                                 QString::number(p.progress),
                                 sqlStr(db, p.description));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

bool ProjectDao::findById(quint32 id, Project *out, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_project WHERE id = %2")
                            .arg(kColumns)
                            .arg(id);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return false;
    if (!rs.next())
        return false;
    if (out)
        *out = rowToProject(rs);
    return true;
}

QVector<Project> ProjectDao::find(const ProjectFilter &filter, int limit, int offset, QString *error)
{
    QVector<Project> list;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_project%2 ORDER BY id DESC LIMIT %3 OFFSET %4")
                            .arg(kColumns, buildWhere(filter))
                            .arg(limit)
                            .arg(offset);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return list;
    while (rs.next())
        list.append(rowToProject(rs));
    return list;
}

int ProjectDao::count(const ProjectFilter &filter, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT COUNT(*) FROM mes_project%1").arg(buildWhere(filter));
    DbResult rs = db.query(sql, error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}

bool ProjectDao::update(const Project &p, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "UPDATE mes_project SET project_no='%1', name=%2, status='%3', owner=%4, "
                            "start_date=%5, end_date=%6, progress=%7, description=%8 WHERE id=%9")
                            .arg(db.escape(p.projectNo),
                                 sqlStr(db, p.name),
                                 db.escape(p.status),
                                 sqlStr(db, p.owner),
                                 sqlDate(p.startDate),
                                 sqlDate(p.endDate),
                                 QString::number(p.progress),
                                 sqlStr(db, p.description),
                                 QString::number(p.id));
    return db.execute(sql, nullptr, error);
}

bool ProjectDao::remove(quint32 id, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    return db.execute(QStringLiteral("DELETE FROM mes_project WHERE id = %1").arg(id), nullptr, error);
}

int ProjectDao::countByProjectNo(const QString &projectNo, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT COUNT(*) FROM mes_project WHERE project_no = '%1'")
                            .arg(db.escape(projectNo));
    DbResult rs = db.query(sql, error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}
