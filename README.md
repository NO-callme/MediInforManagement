# 医疗工程信息管理系统（MediInforManagement）

基于 Qt 6（Widgets）+ C++17 + MySQL 的桌面端医疗工程信息管理系统。

## 技术栈

- UI：Qt 6.12（Widgets）
- 语言：C++17
- 构建：CMake + Ninja
- 编译器：MinGW 13.1（`D:/Qt/Tools/mingw1310_64`，与 Qt 6.12 同套工具链）
- 数据库：MySQL 8.x

## 构建

命令行（Debug）：

```bash
cmake --preset mingw-debug
cmake --build --preset debug
```

Release 同理，换成 `mingw-release` / `release`。

也可直接用 Qt Creator 打开 `CMakeLists.txt`，选择 Qt 6.12.0 MinGW 套件构建。

> 注意：本机默认 `g++` 是 GCC 15.2（`C:/mingw64`），与 Qt 的 GCC 13.1 ABI 不匹配，
> 构建必须使用 `D:/Qt/Tools/mingw1310_64`，已通过 `CMakePresets.json` 固定。

## 目录结构

```
src/
├─ main.cpp
├─ app/       # 应用装配、全局服务初始化
├─ ui/        # 窗口与页面
├─ service/   # 业务逻辑
├─ dao/       # 数据访问
├─ model/     # 实体与 DTO
├─ excel/     # Excel 导入
├─ ocr/       # 图像识别（抽象接口 + 实现）
├─ chart/     # 报表/图表
└─ common/    # 配置、日志、工具
resources/    # 图标、样式表、模板
sql/          # 建库建表脚本、种子数据
tests/        # 单元/集成测试
```

## 开发进度

- [x] P0 环境准备 + 工程骨架（可编译运行的空白主窗口）
- [x] P1 数据层（建表脚本 + DatabaseManager + UserDao，libmysql C API，CRUD 自测通过）
- [x] P2 病例与导入（QXlsx 内嵌 + 病例管理页 + Excel 导入，20 条示例数据验证通过）
- [x] P3 项目管理（项目 CRUD + 里程碑/成员/设备测试详情，逾期高亮预警）
- [ ] P4 可视化
- [ ] P5 图像识别
- [ ] P6 集成联调
- [ ] P7 测试优化
- [ ] P8 二次开发
