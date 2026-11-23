// ============================================================================
// 文件功能：数据库连接管理器的实现，基于 MySQL 的 libmysql C API 封装。
// ============================================================================
#include "DatabaseManager.h"

#include <QDebug>
#include <QSettings>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager mgr;
    return mgr;
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::loadConfig(const QString &path)
{
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Database"));
    host_ = settings.value(QStringLiteral("host"), QStringLiteral("localhost")).toString();
    port_ = static_cast<quint16>(settings.value(QStringLiteral("port"), 3306).toUInt());
    database_ = settings.value(QStringLiteral("database")).toString();
    username_ = settings.value(QStringLiteral("username"), QStringLiteral("root")).toString();
    password_ = settings.value(QStringLiteral("password")).toString();
    settings.endGroup();

    if (database_.isEmpty()) {
        qWarning() << "[DatabaseManager] config 缺少 database 字段:" << path;
        return false;
    }
    return true;
}

bool DatabaseManager::connect(QString *error)
{
    QMutexLocker locker(&mutex_);

    if (conn_) {
        mysql_close(conn_);
        conn_ = nullptr;
    }

    conn_ = mysql_init(nullptr);
    if (!conn_) {
        if (error)
            *error = QStringLiteral("mysql_init 失败");
        return false;
    }

    // 统一使用 utf8mb4，避免中文乱码
    mysql_options(conn_, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    // 设置连接超时（秒），避免断线重连时长时间阻塞界面
    unsigned int connectTimeout = 3;
    mysql_options(conn_, MYSQL_OPT_CONNECT_TIMEOUT, &connectTimeout);

    const QByteArray host = host_.toUtf8();
    const QByteArray user = username_.toUtf8();
    const QByteArray pass = password_.toUtf8();
    const QByteArray db = database_.toUtf8();

    if (!mysql_real_connect(conn_, host.constData(), user.constData(), pass.constData(),
                            db.constData(), port_, nullptr, 0)) {
        if (error)
            *error = QString::fromUtf8(mysql_error(conn_));
        mysql_close(conn_);
        conn_ = nullptr;
        return false;
    }
    return true;
}

void DatabaseManager::close()
{
    QMutexLocker locker(&mutex_);
    if (conn_) {
        mysql_close(conn_);
        conn_ = nullptr;
    }
}

bool DatabaseManager::isConnected() const
{
    QMutexLocker locker(&mutex_);
    return conn_ != nullptr;
}

bool DatabaseManager::ensureConnected()
{
    {
        QMutexLocker locker(&mutex_);
        if (conn_ && mysql_ping(conn_) == 0)
            return true;
    }
    return connect();
}

bool DatabaseManager::execute(const QString &sql, quint64 *affectedRows, QString *error)
{
    QMutexLocker locker(&mutex_);
    if (!conn_) {
        if (error)
            *error = QStringLiteral("未连接数据库");
        return false;
    }

    const QByteArray sqlBytes = sql.toUtf8();
    if (mysql_real_query(conn_, sqlBytes.constData(), sqlBytes.size()) != 0) {
        if (error)
            *error = QString::fromUtf8(mysql_error(conn_));
        return false;
    }
    if (affectedRows)
        *affectedRows = mysql_affected_rows(conn_);
    return true;
}

bool DatabaseManager::beginTransaction()
{
    QMutexLocker locker(&mutex_);
    if (!conn_)
        return false;
    return mysql_query(conn_, "START TRANSACTION") == 0;
}

bool DatabaseManager::commit()
{
    QMutexLocker locker(&mutex_);
    if (!conn_)
        return false;
    return mysql_query(conn_, "COMMIT") == 0;
}

bool DatabaseManager::rollback()
{
    QMutexLocker locker(&mutex_);
    if (!conn_)
        return false;
    return mysql_query(conn_, "ROLLBACK") == 0;
}

DbResult DatabaseManager::query(const QString &sql, QString *error)
{
    QMutexLocker locker(&mutex_);
    if (!conn_) {
        if (error)
            *error = QStringLiteral("未连接数据库");
        return DbResult();
    }

    const QByteArray sqlBytes = sql.toUtf8();
    if (mysql_real_query(conn_, sqlBytes.constData(), sqlBytes.size()) != 0) {
        if (error)
            *error = QString::fromUtf8(mysql_error(conn_));
        return DbResult();
    }

    MYSQL_RES *res = mysql_store_result(conn_);
    if (!res && mysql_field_count(conn_) != 0) {
        if (error)
            *error = QString::fromUtf8(mysql_error(conn_));
        return DbResult();
    }
    return DbResult(res);
}

QString DatabaseManager::escape(const QString &value) const
{
    QMutexLocker locker(&mutex_);
    if (!conn_)
        return value;
    const QByteArray utf8 = value.toUtf8();
    QByteArray buf(utf8.size() * 2 + 1, '\0');
    const unsigned long len =
        mysql_real_escape_string(conn_, buf.data(), utf8.constData(), utf8.size());
    return QString::fromUtf8(buf.constData(), static_cast<int>(len));
}

quint64 DatabaseManager::lastInsertId() const
{
    QMutexLocker locker(&mutex_);
    return conn_ ? mysql_insert_id(conn_) : 0;
}

QString DatabaseManager::lastError() const
{
    QMutexLocker locker(&mutex_);
    if (!conn_)
        return QStringLiteral("未连接数据库");
    return QString::fromUtf8(mysql_error(conn_));
}

QString DatabaseManager::host() const
{
    return host_;
}

quint16 DatabaseManager::port() const
{
    return port_;
}

QString DatabaseManager::database() const
{
    return database_;
}
