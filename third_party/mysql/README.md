# MySQL MinGW 导入库

`lib/libmysql.a` 由 MySQL 8.0.45 的 `libmysql.dll` 经 MinGW 工具生成，供本项目在 MinGW 下链接 libmysql C API 使用。

## 为什么需要

Qt 6.12 预编译版（MinGW）未附带 `qsqlmysql.dll` 驱动，因此本项目 DAO 层直接使用 libmysql C API，需要 MinGW 能链接的导入库。

## 生成步骤（MySQL 版本升级后需重新生成）

```bash
cd third_party/mysql/lib
/d/Qt/Tools/mingw1310_64/bin/gendef.exe "C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.dll"
/d/Qt/Tools/mingw1310_64/bin/dlltool.exe -d libmysql.def -l libmysql.a -D libmysql.dll
```

## 依赖说明

- **头文件**：`C:/Program Files/MySQL/MySQL Server 8.0/include`（见 CMakeLists.txt 的 `MYSQL_INCLUDE_DIR`）。
- **运行时 DLL**：`libmysql.dll`（位于 `MySQL Server 8.0/lib`）需在 PATH 中，或复制到可执行文件同目录。
