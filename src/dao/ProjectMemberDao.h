// ============================================================================
// 文件功能：mes_project_member 表的数据访问对象（DAO）声明，维护项目成员。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/ProjectMember.h"

class ProjectMemberDao
{
public:
    bool insert(const ProjectMember &m, quint32 *newId = nullptr, QString *error = nullptr);
    QVector<ProjectMember> findByProject(quint32 projectId, QString *error = nullptr);
    bool update(const ProjectMember &m, QString *error = nullptr);
    bool remove(quint32 id, QString *error = nullptr);
    void removeByProject(quint32 projectId);
};
