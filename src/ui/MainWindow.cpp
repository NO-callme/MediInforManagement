#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("医疗工程信息管理系统"));
    resize(1200, 800);
}

MainWindow::~MainWindow() = default;
