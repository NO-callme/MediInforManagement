// ============================================================================
// 文件功能：主窗口类 MainWindow 的实现。当前为空白窗口，仅设置标题与初始尺寸。
// ============================================================================
#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("医疗工程信息管理系统"));
    resize(1200, 800);
}

MainWindow::~MainWindow() = default;
