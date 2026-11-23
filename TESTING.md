# 测试报告

## 1. 自动化测试（Qt Test）

### 测试用例

| 测试程序 | 类型 | 覆盖内容 | 依赖数据库 |
| --- | --- | --- | --- |
| `tst_excelparser` | 单元测试 | Excel 解析器：文件缺失处理、逐行校验（有效/非法行） | 否 |
| `tst_ocr` | 单元测试 | 模拟 OCR 引擎：成功 / 失败 / 文件缺失三种情形 | 否 |
| `tst_dao` | 集成测试 | 项目 CRUD、里程碑 CRUD、报表聚合查询 | 是 |
| `db_selftest` | 集成测试 | UserDao 完整 CRUD | 是 |
| `import_selftest` | 集成测试 | Excel 导入 → 事务入库 → 导入记录 全链路 | 是 |

### 运行方式

```bash
# 方式一：CTest
cmake --build --preset debug
cd build/debug
ctest --output-on-failure

# 方式二：直接运行
./build/debug/tst_excelparser.exe
./build/debug/tst_ocr.exe
./build/debug/tst_dao.exe
```

### 测试结果（2026-11-23）

| 测试程序 | 结果 |
| --- | --- |
| tst_excelparser | 4 passed, 0 failed |
| tst_ocr | 5 passed, 0 failed |
| tst_dao | 5 passed, 0 failed |
| **合计** | **14 passed, 0 failed** |

## 2. 手动测试清单（界面）

- [ ] 程序启动并自动连接数据库（状态栏显示「数据库：已连接」）
- [ ] 病例管理：导入 Excel → 列表显示 → 筛选 → 分页 → 增删改
- [ ] 项目管理：新建项目 → 详情维护里程碑/成员/设备测试 → 删除级联清理
- [ ] 数据报表：4 张图表显示正常，导出图片
- [ ] 图像识别：选图 → 识别 → 手动修正 → 保存入库 → 历史记录
- [ ] 断线重连：断开 MySQL 后 10 秒内自动重连，状态栏恢复「已连接」

## 3. 性能优化说明

- **导入查重**：由「逐行查询」优化为「分批批量查询」（每批 500 条），万级数据导入性能显著提升。
- **列表展示**：数据库分页 + 模型/视图，避免大数据量卡顿。
- **数据库重连**：连接超时设为 3 秒，心跳重连避免界面长时间卡死。
