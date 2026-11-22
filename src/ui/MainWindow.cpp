// ============================================================================
// 文件功能：主窗口实现。构建左侧导航与右侧页面堆叠，建立导航切换。
// ============================================================================
#include "MainWindow.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

#include "CaseManagementWidget.h"
#include "OcrRecognitionWidget.h"
#include "ProjectManagementWidget.h"
#include "ReportCenterWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("医疗工程信息管理系统"));
    resize(1200, 800);

    navList_ = new QListWidget(this);
    navList_->setFixedWidth(160);
    navList_->addItem(tr("病例管理"));
    navList_->addItem(tr("项目管理"));
    navList_->addItem(tr("数据报表"));
    navList_->addItem(tr("图像识别"));
    // 后续阶段（P6+）在此追加：系统设置等

    stack_ = new QStackedWidget(this);
    casePage_ = new CaseManagementWidget(this);
    stack_->addWidget(casePage_);

    projectPage_ = new ProjectManagementWidget(this);
    stack_->addWidget(projectPage_);

    reportPage_ = new ReportCenterWidget(this);
    stack_->addWidget(reportPage_);

    ocrPage_ = new OcrRecognitionWidget(this);
    stack_->addWidget(ocrPage_);

    auto *central = new QWidget(this);
    auto *layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(navList_);
    layout->addWidget(stack_, 1);
    setCentralWidget(central);

    connect(navList_, &QListWidget::currentRowChanged, stack_, &QStackedWidget::setCurrentIndex);
    navList_->setCurrentRow(0);
}

MainWindow::~MainWindow() = default;
