// ============================================================================
// 文件功能：mes_project_milestone 表的数据访问对象（DAO）声明，维护项目里程碑。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/Milestone.h"

class MilestoneDao
{
public:
    bool insert(const Milestone &m, quint32 *newId = nullptr, QString *error = nullptr);
    QVector<Milestone> findByProject(quint32 projectId, QString *error = nullptr);
    bool update(const Milestone &m, QString *error = nullptr);
    bool remove(quint32 id, QString *error = nullptr);
    void removeByProject(quint32 projectId); // 删除项目时级联清理
};
