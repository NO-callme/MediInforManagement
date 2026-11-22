// ============================================================================
// 文件功能：OCR 引擎工厂实现。读取 config.ini 的 [ocr] engine 字段。
// ============================================================================
#include "OcrEngineFactory.h"

#include <QDebug>
#include <QSettings>

#include "MockOcrEngine.h"

std::unique_ptr<OcrEngine> OcrEngineFactory::create()
{
    QSettings settings(QStringLiteral("config.ini"), QSettings::IniFormat);
    const QString engine = settings.value(QStringLiteral("ocr/engine"), QStringLiteral("mock")).toString();

    if (engine == QStringLiteral("mock"))
        return std::make_unique<MockOcrEngine>();

    // 未来接入云端：在此根据 engine 返回百度/腾讯等实现
    qWarning() << "未实现的 OCR 引擎类型:" << engine << "，回退到 mock";
    return std::make_unique<MockOcrEngine>();
}
