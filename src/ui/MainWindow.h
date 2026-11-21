// ============================================================================
// 文件功能：主窗口类 MainWindow 的声明。左侧导航 + 右侧页面堆叠（QStackedWidget），
//           当前阶段含「病例管理」页，后续阶段增量加入其它功能页。
// ============================================================================
#pragma once

#include <QMainWindow>

class QListWidget;
class QStackedWidget;
class CaseManagementWidget;
class ProjectManagementWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    QListWidget *navList_;
    QStackedWidget *stack_;
    CaseManagementWidget *casePage_;
    ProjectManagementWidget *projectPage_;
};
