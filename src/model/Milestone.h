// ============================================================================
// 文件功能：项目里程碑实体 Milestone 的定义，对应数据库表 mes_project_milestone。
// ============================================================================
#pragma once

#include <QDate>
#include <QString>

struct Milestone
{
    quint32 id = 0;
    quint32 projectId = 0;  // 所属项目
    QString name;           // 里程碑名称
    QDate planDate;         // 计划日期
    QDate actualDate;       // 实际日期
    QString status;         // 未开始 / 进行中 / 已完成
};
