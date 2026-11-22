// ============================================================================
// 文件功能：OCR 引擎工厂声明。根据 config.ini 的 [ocr] engine 配置返回对应引擎，
//           默认 mock；未来接入云端时在此扩展。
// ============================================================================
#pragma once

#include <memory>

#include "OcrEngine.h"

class OcrEngineFactory
{
public:
    // 根据配置返回引擎实例；未知类型回退到 mock
    static std::unique_ptr<OcrEngine> create();
};
