// ============================================================================
// 文件功能：OCR 引擎抽象接口定义。OcrResult 表示识别结果，OcrEngine 为所有
//           识别实现（模拟/云端）的统一抽象，便于替换。
// ============================================================================
#pragma once

#include <QMap>
#include <QString>

// 一次识别的结果
struct OcrResult
{
    bool ok = false;                 // 识别是否成功
    QString error;                   // 失败原因
    QMap<QString, QString> fields;   // 识别出的字段（字段名 -> 值）
    double confidence = 0.0;         // 置信度 0~1
};

// OCR 引擎抽象接口
class OcrEngine
{
public:
    virtual ~OcrEngine() = default;

    // 识别一张图片，返回结构化字段结果
    virtual OcrResult recognize(const QString &imagePath) = 0;

    virtual QString name() const = 0;
};
