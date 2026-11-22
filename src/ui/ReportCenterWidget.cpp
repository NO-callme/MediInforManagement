// ============================================================================
// 文件功能：报表中心页实现。用 Qt Charts 绘制柱状图/饼图/折线图。
// ============================================================================
#include "ReportCenterWidget.h"

#include <QtCharts/QChart>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>

#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

#include "dao/ReportDao.h"

namespace {

QChart *buildBarChart(const QVector<StatItem> &data, const QString &title)
{
    auto *chart = new QChart();
    chart->setTitle(title);

    auto *set = new QBarSet(QStringLiteral("数量"));
    QStringList cats;
    double maxVal = 0;
    for (const StatItem &it : data) {
        *set << it.value;
        cats << it.label;
        maxVal = qMax(maxVal, it.value);
    }

    auto *series = new QBarSeries();
    series->append(set);
    chart->addSeries(series);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(cats);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, maxVal + 1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    return chart;
}

QChart *buildPieChart(const QVector<StatItem> &data, const QString &title)
{
    auto *chart = new QChart();
    chart->setTitle(title);
    auto *series = new QPieSeries();
    for (const StatItem &it : data)
        series->append(it.label, it.value);
    chart->addSeries(series);
    return chart;
}

QChart *buildLineChart(const QVector<StatItem> &data, const QString &title)
{
    auto *chart = new QChart();
    chart->setTitle(title);

    auto *series = new QLineSeries();
    for (int i = 0; i < data.size(); ++i)
        series->append(i, data.at(i).value);
    chart->addSeries(series);

    auto *axisX = new QCategoryAxis();
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for (int i = 0; i < data.size(); ++i)
        axisX->append(data.at(i).label, i);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText(QStringLiteral("通过率(%)"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    return chart;
}

} // namespace

ReportCenterWidget::ReportCenterWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *refreshBtn = new QPushButton(tr("刷新"), this);
    auto *exportBtn = new QPushButton(tr("导出图片"), this);
    connect(refreshBtn, &QPushButton::clicked, this, &ReportCenterWidget::refresh);
    connect(exportBtn, &QPushButton::clicked, this, &ReportCenterWidget::onExport);

    auto *toolbar = new QHBoxLayout;
    toolbar->addWidget(refreshBtn);
    toolbar->addWidget(exportBtn);
    toolbar->addStretch();

    deptView_ = new QChartView(this);
    deptView_->setRenderHint(QPainter::Antialiasing);
    monthView_ = new QChartView(this);
    monthView_->setRenderHint(QPainter::Antialiasing);
    statusView_ = new QChartView(this);
    statusView_->setRenderHint(QPainter::Antialiasing);
    passRateView_ = new QChartView(this);
    passRateView_->setRenderHint(QPainter::Antialiasing);

    auto *grid = new QGridLayout;
    grid->addWidget(deptView_, 0, 0);
    grid->addWidget(monthView_, 0, 1);
    grid->addWidget(statusView_, 1, 0);
    grid->addWidget(passRateView_, 1, 1);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(toolbar);
    layout->addLayout(grid, 1);

    refresh();
}

void ReportCenterWidget::refresh()
{
    ReportDao dao;
    QString err;

    deptView_->setChart(buildBarChart(dao.patientCasesByDepartment(&err), tr("病例按科室统计")));
    monthView_->setChart(buildBarChart(dao.patientCasesByMonth(&err), tr("病例按入院月份统计")));
    statusView_->setChart(buildPieChart(dao.projectStatusDistribution(&err), tr("项目状态分布")));
    passRateView_->setChart(buildLineChart(dao.testPassRateByMonth(&err), tr("设备测试通过率（按月）")));
}

void ReportCenterWidget::onExport()
{
    const QString dir = QFileDialog::getExistingDirectory(this, tr("选择导出目录"));
    if (dir.isEmpty())
        return;

    const QList<QPair<QString, QChartView *>> charts = {
        {QStringLiteral("病例按科室"), deptView_},
        {QStringLiteral("病例按月份"), monthView_},
        {QStringLiteral("项目状态"), statusView_},
        {QStringLiteral("测试通过率"), passRateView_},
    };
    int ok = 0;
    for (const auto &c : charts) {
        const QString path = dir + QStringLiteral("/") + c.first + QStringLiteral(".png");
        if (c.second->grab().save(path))
            ++ok;
    }
    QMessageBox::information(this, tr("导出完成"), tr("已导出 %1 张图片到：\n%2").arg(ok).arg(dir));
}
