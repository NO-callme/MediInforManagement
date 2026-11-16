// ============================================================================
// 文件功能：程序入口。初始化 QApplication，创建并显示主窗口，进入 Qt 事件循环。
// ============================================================================
#include <QApplication>

#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("MediInforManagement"));
    app.setOrganizationName(QStringLiteral("MedicalEngineering"));

    MainWindow window;
    window.show();

    return app.exec();
}
