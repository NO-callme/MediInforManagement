// ============================================================================
// 文件功能：报表统计 DAO 声明。执行聚合查询，为数据可视化提供统计数据。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

// 统计数据项：标签 + 数值
struct StatItem
{
    QString label;
    double value;
};

class ReportDao
{
public:
    QVector<StatItem> patientCasesByDepartment(QString *error = nullptr); // 病例按科室
    QVector<StatItem> patientCasesByMonth(QString *error = nullptr);      // 病例按入院月份
    QVector<StatItem> projectStatusDistribution(QString *error = nullptr);// 项目状态分布
    QVector<StatItem> testPassRateByMonth(QString *error = nullptr);      // 测试通过率按月(0-100)
};
