// ============================================================================
// 文件功能：模拟 OCR 引擎实现。根据文件名哈希生成确定性的虚构识别结果。
// ============================================================================
#include "MockOcrEngine.h"

#include <QFileInfo>

OcrResult MockOcrEngine::recognize(const QString &imagePath)
{
    OcrResult r;
    const QFileInfo fi(imagePath);
    if (!fi.exists()) {
        r.error = QStringLiteral("文件不存在：%1").arg(imagePath);
        return r;
    }

    const QString base = fi.completeBaseName();
    // 用于演示「识别失败重试」：文件名含 fail / 失败 时返回失败
    if (base.contains(QStringLiteral("fail")) || base.contains(QStringLiteral("失败"))) {
        r.error = QStringLiteral("模拟识别失败（文件名含 fail/失败，用于演示重试）");
        return r;
    }

    r.ok = true;
    r.confidence = 0.90 + (qHash(base) % 10) / 100.0;
    r.fields.insert(QStringLiteral("病例号"), QStringLiteral("OCR%1").arg(qHash(base) % 90000 + 10000));
    r.fields.insert(QStringLiteral("姓名"), QStringLiteral("模拟患者"));
    r.fields.insert(QStringLiteral("性别"), (qHash(base) % 2 == 0) ? QStringLiteral("男") : QStringLiteral("女"));
    r.fields.insert(QStringLiteral("科室"), QStringLiteral("心内科"));
    r.fields.insert(QStringLiteral("诊断"), QStringLiteral("高血压"));
    return r;
}
