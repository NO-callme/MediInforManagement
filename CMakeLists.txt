cmake_minimum_required(VERSION 3.21)

project(MediInforManagement
    VERSION 0.1.0
    DESCRIPTION "医疗工程信息管理系统"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 元对象/资源/UI 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 当前仅 P0：Widgets。后续按模块增量引入（Sql、Charts 等）。
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
)

add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})

# 统一以 src/ 为包含根，业务代码可写 #include "ui/..." / "dao/..." 等
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
