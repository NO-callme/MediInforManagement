#pragma once

#include <QString>
#include <QVector>

#include "model/User.h"

// mes_user 数据访问对象：CRUD + 分页查询
class UserDao
{
public:
    bool insert(const User &user, quint32 *newId = nullptr, QString *error = nullptr);
    bool findById(quint32 id, User *out = nullptr, QString *error = nullptr);
    bool findByUsername(const QString &username, User *out = nullptr, QString *error = nullptr);
    QVector<User> findAll(int limit = 100, int offset = 0, QString *error = nullptr);
    bool update(const User &user, QString *error = nullptr);
    bool remove(quint32 id, QString *error = nullptr);
    int count(QString *error = nullptr);
};
