-- ============================================================
-- 医疗工程信息管理系统 建库建表脚本
-- 数据库：medical_engineering  字符集：utf8mb4
-- 表前缀：mes_（Medical Engineering System）
-- 执行方式：mysql -uroot -p < init.sql
-- ============================================================

-- 确保连接使用 utf8mb4，否则中文默认值/注释会报 Invalid default value
SET NAMES utf8mb4;

CREATE DATABASE IF NOT EXISTS medical_engineering
    DEFAULT CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE medical_engineering;

-- 用户与登录
CREATE TABLE IF NOT EXISTS mes_user (
    id          INT UNSIGNED     NOT NULL AUTO_INCREMENT,
    username    VARCHAR(50)      NOT NULL COMMENT '登录名',
    password    VARCHAR(255)     NOT NULL COMMENT '密码（暂为明文，后续加盐哈希）',
    role        VARCHAR(20)      NOT NULL DEFAULT 'operator' COMMENT '角色：admin/operator',
    real_name   VARCHAR(50)      DEFAULT NULL COMMENT '真实姓名',
    create_time DATETIME         NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    UNIQUE KEY uk_username (username)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '用户与登录';

-- 医疗病例（Excel 导入主数据）
CREATE TABLE IF NOT EXISTS mes_patient_case (
    id               INT UNSIGNED NOT NULL AUTO_INCREMENT,
    case_no          VARCHAR(50)  NOT NULL COMMENT '病例号',
    patient_name     VARCHAR(50)  DEFAULT NULL COMMENT '姓名',
    gender           VARCHAR(10)  DEFAULT NULL COMMENT '性别',
    age              INT          DEFAULT NULL COMMENT '年龄',
    department       VARCHAR(100) DEFAULT NULL COMMENT '科室',
    diagnosis        VARCHAR(500) DEFAULT NULL COMMENT '诊断',
    admit_date       DATE         DEFAULT NULL COMMENT '入院日期',
    discharge_date   DATE         DEFAULT NULL COMMENT '出院日期',
    import_record_id INT UNSIGNED DEFAULT NULL COMMENT '所属导入批次',
    create_time      DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    UNIQUE KEY uk_case_no (case_no),
    KEY idx_import_record (import_record_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '医疗病例';

-- 导入批次记录
CREATE TABLE IF NOT EXISTS mes_import_record (
    id           INT UNSIGNED NOT NULL AUTO_INCREMENT,
    file_name    VARCHAR(255) DEFAULT NULL COMMENT '源文件名',
    total_rows   INT          NOT NULL DEFAULT 0 COMMENT '总行数',
    success_rows INT          NOT NULL DEFAULT 0 COMMENT '成功行数',
    failed_rows  INT          NOT NULL DEFAULT 0 COMMENT '失败行数',
    import_time  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    operator     VARCHAR(50)  DEFAULT NULL COMMENT '操作人',
    PRIMARY KEY (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '导入批次记录';

-- 医疗工程项目
CREATE TABLE IF NOT EXISTS mes_project (
    id          INT UNSIGNED NOT NULL AUTO_INCREMENT,
    project_no  VARCHAR(50)  NOT NULL COMMENT '项目编号',
    name        VARCHAR(200) NOT NULL COMMENT '项目名称',
    status      VARCHAR(20)  NOT NULL DEFAULT '进行中' COMMENT '进行中/已完成/已延期',
    owner       VARCHAR(50)  DEFAULT NULL COMMENT '负责人',
    start_date  DATE         DEFAULT NULL COMMENT '开始日期',
    end_date    DATE         DEFAULT NULL COMMENT '结束日期',
    progress    TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '进度 0-100',
    description TEXT COMMENT '项目描述',
    PRIMARY KEY (id),
    UNIQUE KEY uk_project_no (project_no)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '医疗工程项目';

-- 项目里程碑/时间节点
CREATE TABLE IF NOT EXISTS mes_project_milestone (
    id          INT UNSIGNED NOT NULL AUTO_INCREMENT,
    project_id  INT UNSIGNED NOT NULL COMMENT '所属项目',
    name        VARCHAR(200) NOT NULL COMMENT '里程碑名称',
    plan_date   DATE         DEFAULT NULL COMMENT '计划日期',
    actual_date DATE         DEFAULT NULL COMMENT '实际日期',
    status      VARCHAR(20)  DEFAULT '未开始' COMMENT '状态',
    PRIMARY KEY (id),
    KEY idx_project (project_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '项目里程碑';

-- 项目人员配置
CREATE TABLE IF NOT EXISTS mes_project_member (
    id         INT UNSIGNED NOT NULL AUTO_INCREMENT,
    project_id INT UNSIGNED NOT NULL COMMENT '所属项目',
    name       VARCHAR(50)  NOT NULL COMMENT '姓名',
    role       VARCHAR(50)  DEFAULT NULL COMMENT '角色/分工',
    phone      VARCHAR(30)  DEFAULT NULL COMMENT '电话',
    remark     VARCHAR(500) DEFAULT NULL COMMENT '备注',
    PRIMARY KEY (id),
    KEY idx_project (project_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '项目人员配置';

-- 设备测试数据
CREATE TABLE IF NOT EXISTS mes_equipment_test (
    id             INT UNSIGNED NOT NULL AUTO_INCREMENT,
    project_id     INT UNSIGNED DEFAULT NULL COMMENT '所属项目',
    equipment_name VARCHAR(100) DEFAULT NULL COMMENT '设备名称',
    model          VARCHAR(100) DEFAULT NULL COMMENT '型号',
    test_item      VARCHAR(200) DEFAULT NULL COMMENT '测试项',
    test_result    VARCHAR(500) DEFAULT NULL COMMENT '测试结果',
    test_date      DATE         DEFAULT NULL COMMENT '测试日期',
    tester         VARCHAR(50)  DEFAULT NULL COMMENT '测试人',
    PRIMARY KEY (id),
    KEY idx_project (project_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '设备测试数据';

-- 设备台账（二次开发新增）
CREATE TABLE IF NOT EXISTS mes_equipment (
    id            INT UNSIGNED NOT NULL AUTO_INCREMENT,
    equipment_no  VARCHAR(50)  NOT NULL COMMENT '设备编号',
    name          VARCHAR(100) NOT NULL COMMENT '设备名称',
    model         VARCHAR(100) DEFAULT NULL COMMENT '型号',
    category      VARCHAR(50)  DEFAULT NULL COMMENT '分类',
    status        VARCHAR(20)  NOT NULL DEFAULT '在用' COMMENT '在用/维修/报废',
    purchase_date DATE         DEFAULT NULL COMMENT '采购日期',
    department    VARCHAR(100) DEFAULT NULL COMMENT '所属科室',
    remark        VARCHAR(500) DEFAULT NULL COMMENT '备注',
    create_time   DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    UNIQUE KEY uk_equipment_no (equipment_no)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '设备台账';

-- 图像识别记录与结果
CREATE TABLE IF NOT EXISTS mes_ocr_record (
    id              INT UNSIGNED NOT NULL AUTO_INCREMENT,
    image_path      VARCHAR(500) DEFAULT NULL COMMENT '图片路径',
    recognized_json JSON         DEFAULT NULL COMMENT '识别结果 JSON',
    status          VARCHAR(20)  NOT NULL DEFAULT '待确认' COMMENT '成功/失败/待确认',
    create_time     DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    operator        VARCHAR(50)  DEFAULT NULL COMMENT '操作人',
    PRIMARY KEY (id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COMMENT = '图像识别记录';

-- 种子数据：默认管理员
INSERT INTO mes_user (username, password, role, real_name)
VALUES ('admin', 'admin123', 'admin', '系统管理员')
ON DUPLICATE KEY UPDATE username = username;
