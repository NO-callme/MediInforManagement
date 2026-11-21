// ============================================================================
// 文件功能：医疗工程项目实体 Project 与查询过滤条件 ProjectFilter 的定义，
//           对应数据库表 mes_project。
// ============================================================================
#pragma once

#include <QDate>
#include <QString>

struct Project
{
    quint32 id = 0;
    QString projectNo;   // 项目编号
    QString name;        // 项目名称
    QString status;      // 进行中 / 已完成 / 已延期
    QString owner;       // 负责人
    QDate startDate;     // 开始日期
    QDate endDate;       // 结束日期
    int progress = 0;    // 进度百分比 0-100
    QString description; // 项目描述
};

struct ProjectFilter
{
    QString keyword;  // 模糊匹配：项目编号/名称/负责人
    QString status;   // 精确状态（空=全部）
};
