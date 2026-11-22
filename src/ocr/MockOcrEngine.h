// ============================================================================
// 文件功能：模拟 OCR 引擎声明。返回虚构的识别结果，用于无需账号即可演示
//           完整识别流程；文件名含 fail/失败 时模拟识别失败。
// ============================================================================
#pragma once

#include "OcrEngine.h"

class MockOcrEngine : public OcrEngine
{
public:
    OcrResult recognize(const QString &imagePath) override;
    QString name() const override { return QStringLiteral("mock"); }
};
