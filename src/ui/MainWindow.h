// ============================================================================
// 文件功能：主窗口类 MainWindow 的声明（派生自 QMainWindow）。
// ============================================================================
#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
};
