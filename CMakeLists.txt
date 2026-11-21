cmake_minimum_required(VERSION 3.21)

project(MediInforManagement
    VERSION 0.1.0
    DESCRIPTION "医疗工程信息管理系统"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 导出 compile_commands.json，供 IDE（IntelliSense/clangd）读取真实编译参数
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Qt 元对象/资源/UI 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ---- QXlsx（Excel 读写，源码内嵌）----
# QXlsx 用到 Qt 的 GuiPrivate 私有头，抑制 Qt 的私有模块警告
set(QT_NO_PRIVATE_MODULE_WARNING ON)
add_subdirectory(third_party/qxlsx/QXlsx)

# ---- MySQL（libmysql C API）----
# 头文件目录（MySQL Server 安装自带）
set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include" CACHE PATH "MySQL include 目录")
# MinGW 导入库目录（由 libmysql.dll 经 gendef+dlltool 生成，见 third_party/mysql/README）
set(MYSQL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mysql/lib" CACHE PATH "MinGW 导入库目录")

# ---- 核心静态库：model + dao + common（后续加入 service 层）----
add_library(mes_core STATIC
    src/common/DatabaseManager.h
    src/common/DatabaseManager.cpp
    src/common/DbResult.h
    src/common/DbResult.cpp
    src/model/User.h
    src/model/PatientCase.h
    src/model/ImportRecord.h
    src/model/Project.h
    src/model/Milestone.h
    src/model/ProjectMember.h
    src/model/EquipmentTest.h
    src/dao/UserDao.h
    src/dao/UserDao.cpp
    src/dao/PatientCaseDao.h
    src/dao/PatientCaseDao.cpp
    src/dao/ImportRecordDao.h
    src/dao/ImportRecordDao.cpp
    src/dao/ProjectDao.h
    src/dao/ProjectDao.cpp
    src/dao/MilestoneDao.h
    src/dao/MilestoneDao.cpp
    src/dao/ProjectMemberDao.h
    src/dao/ProjectMemberDao.cpp
    src/dao/EquipmentTestDao.h
    src/dao/EquipmentTestDao.cpp
    src/excel/ExcelCaseParser.h
    src/excel/ExcelCaseParser.cpp
    src/service/CaseImportService.h
    src/service/CaseImportService.cpp
)

target_include_directories(mes_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MYSQL_INCLUDE_DIR}
)
target_link_directories(mes_core PUBLIC ${MYSQL_LIB_DIR})
target_link_libraries(mes_core PUBLIC Qt6::Core mysql QXlsx::QXlsx)

# ---- 主程序（GUI）----
add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/CaseManagementWidget.cpp
    src/ui/CaseManagementWidget.h
    src/ui/PatientCaseTableModel.cpp
    src/ui/PatientCaseTableModel.h
    src/ui/CaseEditDialog.cpp
    src/ui/CaseEditDialog.h
    src/ui/ImportDialog.cpp
    src/ui/ImportDialog.h
    src/ui/ProjectTableModel.cpp
    src/ui/ProjectTableModel.h
    src/ui/ProjectEditDialog.cpp
    src/ui/ProjectEditDialog.h
    src/ui/ProjectManagementWidget.cpp
    src/ui/ProjectManagementWidget.h
    src/ui/ProjectDetailDialog.cpp
    src/ui/ProjectDetailDialog.h
    src/ui/MilestoneEditDialog.cpp
    src/ui/MilestoneEditDialog.h
    src/ui/MemberEditDialog.cpp
    src/ui/MemberEditDialog.h
    src/ui/EquipmentTestEditDialog.cpp
    src/ui/EquipmentTestEditDialog.h
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE mes_core Qt6::Widgets)

# ---- DB 自测程序（控制台，验证数据层 CRUD）----
add_executable(db_selftest
    tests/db_selftest.cpp
)
target_link_libraries(db_selftest PRIVATE mes_core Qt6::Core)

# ---- 示例数据生成工具（控制台，用 QXlsx 写示例 .xlsx）----
add_executable(gen_sample_xlsx
    tests/gen_sample_xlsx.cpp
)
target_link_libraries(gen_sample_xlsx PRIVATE mes_core Qt6::Core)

# ---- 导入自测程序（控制台，验证 Excel 导入全链路）----
add_executable(import_selftest
    tests/import_selftest.cpp
)
target_link_libraries(import_selftest PRIVATE mes_core Qt6::Core)
