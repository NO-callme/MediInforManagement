// ============================================================================
// 文件功能：图像识别记录实体 OcrRecord 的定义，对应数据库表 mes_ocr_record。
// ============================================================================
#pragma once

#include <QDateTime>
#include <QString>

struct OcrRecord
{
    quint32 id = 0;
    QString imagePath;       // 图片路径
    QString recognizedJson;  // 识别结果 JSON
    QString status;          // 成功 / 失败 / 待确认
    QDateTime createTime;
    QString operatorName;    // 操作人（对应库字段 operator）
};
