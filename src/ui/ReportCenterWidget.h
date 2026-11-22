// ============================================================================
// 文件功能：报表中心页 ReportCenterWidget 声明。展示 4 张统计图表，支持
//           刷新与导出图片。
// ============================================================================
#pragma once

#include <QWidget>

class QChartView;

class ReportCenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportCenterWidget(QWidget *parent = nullptr);

    void refresh(); // 重新加载统计数据并刷新图表

private slots:
    void onExport();

private:
    QChartView *deptView_;     // 病例按科室柱状图
    QChartView *monthView_;    // 病例按月份柱状图
    QChartView *statusView_;   // 项目状态饼图
    QChartView *passRateView_; // 测试通过率折线图
};
