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
