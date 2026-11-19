# QXlsx（内嵌第三方库）

从 Gitee 镜像（`gitee.com/mirrors/QXlsx`）克隆的 **QXlsx v1.5.1**，仅保留库本体 `QXlsx/` 与 `LICENSE`（示例、多语言文档等已移除）。

- **上游**：https://github.com/QtExcel/QXlsx
- **用途**：本项目 Excel 病例数据的读取/写入（.xlsx）
- **接入方式**：根 `CMakeLists.txt` 中 `add_subdirectory(third_party/qxlsx/QXlsx)`，链接目标 `QXlsx::QXlsx`
- **依赖**：Qt6 Core / Gui（Qt 6.10+ 还需 GuiPrivate 私有模块）
