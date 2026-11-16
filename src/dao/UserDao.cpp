// ============================================================================
// 文件功能：mes_user 表数据访问对象的实现，SQL 经 DatabaseManager 执行。
// ============================================================================
#include "UserDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns = "id, username, password, role, real_name, create_time";

User rowToUser(const DbResult &rs)
{
    User u;
    u.id = rs.value(0).toUInt();
    u.username = rs.value(1);
    u.password = rs.value(2);
    u.role = rs.value(3);
    u.realName = rs.value(4);
    u.createTime = QDateTime::fromString(rs.value(5), QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    return u;
}

} // namespace

bool UserDao::insert(const User &user, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_user (username, password, role, real_name) "
                            "VALUES ('%1', '%2', '%3', '%4')")
                            .arg(db.escape(user.username),
                                 db.escape(user.password),
                                 db.escape(user.role),
                                 db.escape(user.realName));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

bool UserDao::findById(quint32 id, User *out, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_user WHERE id = %2").arg(kColumns).arg(id);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return false;
    if (!rs.next())
        return false; // 未找到
    if (out)
        *out = rowToUser(rs);
    return true;
}

bool UserDao::findByUsername(const QString &username, User *out, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_user WHERE username = '%2'")
                            .arg(kColumns, db.escape(username));
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return false;
    if (!rs.next())
        return false;
    if (out)
        *out = rowToUser(rs);
    return true;
}

QVector<User> UserDao::findAll(int limit, int offset, QString *error)
{
    QVector<User> users;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_user ORDER BY id LIMIT %2 OFFSET %3")
                            .arg(kColumns)
                            .arg(limit)
                            .arg(offset);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return users;
    while (rs.next())
        users.append(rowToUser(rs));
    return users;
}

bool UserDao::update(const User &user, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "UPDATE mes_user SET username='%1', password='%2', role='%3', real_name='%4' "
                            "WHERE id=%5")
                            .arg(db.escape(user.username),
                                 db.escape(user.password),
                                 db.escape(user.role),
                                 db.escape(user.realName))
                            .arg(user.id);
    return db.execute(sql, nullptr, error);
}

bool UserDao::remove(quint32 id, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("DELETE FROM mes_user WHERE id = %1").arg(id);
    return db.execute(sql, nullptr, error);
}

int UserDao::count(QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    DbResult rs = db.query(QStringLiteral("SELECT COUNT(*) FROM mes_user"), error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}
