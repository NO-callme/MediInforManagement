// ============================================================================
// 文件功能：数据库连接管理器（单例）声明。负责读取 config.ini、建立/重连
//           libmysql 连接、执行 SQL（execute/query）、字符串转义与 lastInsertId。
// ============================================================================
#pragma once

#include <QString>
#include <QMutex>

#include <mysql.h>

#include "DbResult.h"

// 数据库连接管理：读取 config.ini、建立/重连 libmysql 连接、执行 SQL
class DatabaseManager
{
public:
    static DatabaseManager &instance();

    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;

    bool loadConfig(const QString &path);

    bool connect(QString *error = nullptr);
    void close();
    bool isConnected() const;
    bool ensureConnected();

    // INSERT/UPDATE/DELETE 等非查询语句
    bool execute(const QString &sql, quint64 *affectedRows = nullptr, QString *error = nullptr);
    // SELECT 查询，返回可迭代结果集（无效表示出错）
    DbResult query(const QString &sql, QString *error = nullptr);

    QString escape(const QString &value) const;   // 防 SQL 注入转义
    quint64 lastInsertId() const;
    QString lastError() const;

    QString host() const;
    quint16 port() const;
    QString database() const;

private:
    DatabaseManager() = default;
    ~DatabaseManager();

    QString host_ = QStringLiteral("localhost");
    quint16 port_ = 3306;
    QString database_;
    QString username_;
    QString password_;

    MYSQL *conn_ = nullptr;
    mutable QMutex mutex_;
};
