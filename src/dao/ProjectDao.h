// ============================================================================
// 文件功能：mes_project 表的数据访问对象（DAO）声明，提供项目的 CRUD、
//           条件筛选与分页查询。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/Project.h"

class ProjectDao
{
public:
    bool insert(const Project &p, quint32 *newId = nullptr, QString *error = nullptr);
    bool findById(quint32 id, Project *out = nullptr, QString *error = nullptr);
    QVector<Project> find(const ProjectFilter &filter, int limit = 100, int offset = 0,
                          QString *error = nullptr);
    int count(const ProjectFilter &filter, QString *error = nullptr);
    bool update(const Project &p, QString *error = nullptr);
    bool remove(quint32 id, QString *error = nullptr);
    int countByProjectNo(const QString &projectNo, QString *error = nullptr); // 查重
};
