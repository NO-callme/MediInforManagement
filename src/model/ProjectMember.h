// ============================================================================
// 文件功能：项目成员实体 ProjectMember 的定义，对应数据库表 mes_project_member。
// ============================================================================
#pragma once

#include <QString>

struct ProjectMember
{
    quint32 id = 0;
    quint32 projectId = 0;  // 所属项目
    QString name;           // 姓名
    QString role;           // 角色/分工
    QString phone;          // 电话
    QString remark;         // 备注
};
