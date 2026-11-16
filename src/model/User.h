#pragma once

#include <QDateTime>
#include <QString>

// 用户实体，对应表 mes_user
struct User
{
    quint32 id = 0;
    QString username;
    QString password;
    QString role;      // admin / operator
    QString realName;
    QDateTime createTime;
};
