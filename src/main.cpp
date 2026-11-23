// ============================================================================
// 文件功能：程序入口。初始化 QApplication，加载配置并连接数据库，
//           创建并显示主窗口，进入 Qt 事件循环。
// ============================================================================
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "common/DatabaseManager.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("MediInforManagement"));
    app.setOrganizationName(QStringLiteral("MedicalEngineering"));

    // 加载全局样式表
    QFile qss(QStringLiteral(":/resources/style.qss"));
    if (qss.open(QFile::ReadOnly | QFile::Text))
        app.setStyleSheet(QString::fromUtf8(qss.readAll()));

    // 连接数据库：优先应用目录下的 config.ini，其次当前工作目录
    QString configPath = QCoreApplication::applicationDirPath() + QStringLiteral("/config.ini");
    if (!QFileInfo::exists(configPath))
        configPath = QStringLiteral("config.ini");

    DatabaseManager &db = DatabaseManager::instance();
    if (!db.loadConfig(configPath)) {
        QMessageBox::warning(nullptr, QStringLiteral("配置错误"),
                             QStringLiteral("无法加载数据库配置：%1").arg(configPath));
    } else {
        QString err;
        if (!db.connect(&err))
            QMessageBox::warning(nullptr, QStringLiteral("数据库连接失败"), err);
    }

    MainWindow window;
    window.show();

    return app.exec();
}
