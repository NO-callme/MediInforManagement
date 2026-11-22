// ============================================================================
// 文件功能：示例图片生成工具。生成 3 张模拟「医疗业务表格」图片，供图像识别
//           功能演示；其中 sample3-fail.png 用于演示识别失败重试。
// ============================================================================
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    const char *names[] = {"sample1.png", "sample2.png", "sample3-fail.png"};
    const int count = sizeof(names) / sizeof(names[0]);

    QDir().mkpath(QStringLiteral("resources/samples"));
    for (int i = 0; i < count; ++i) {
        QImage img(480, 220, QImage::Format_RGB32);
        img.fill(Qt::white);
        QPainter p(&img);
        p.setPen(Qt::black);
        p.setFont(QFont(QStringLiteral("SimHei"), 16));
        p.drawText(30, 60, QStringLiteral("医疗业务表格（示例 %1）").arg(i + 1));
        p.drawText(30, 110, QStringLiteral("病例号：2026%1").arg(100 + i));
        p.drawText(30, 160, QStringLiteral("诊断：高血压"));
        p.end();

        const QString path = QStringLiteral("resources/samples/") + QString::fromUtf8(names[i]);
        if (!img.save(path))
            qCritical() << "保存失败:" << path;
        else
            qInfo() << "已生成:" << path;
    }
    return 0;
}
