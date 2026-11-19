// ============================================================================
// 文件功能：病例实体 PatientCase 与查询过滤条件 PatientCaseFilter 的定义，
//           对应数据库表 mes_patient_case。
// ============================================================================
#pragma once

#include <QDate>
#include <QString>

// 病例实体，对应表 mes_patient_case
struct PatientCase
{
    quint32 id = 0;
    QString caseNo;         // 病例号（必填，唯一）
    QString patientName;    // 姓名
    QString gender;         // 性别（男/女，空=未填）
    int age = -1;           // 年龄（-1 = 未填）
    QString department;     // 科室
    QString diagnosis;      // 诊断
    QDate admitDate;        // 入院日期（无效 = 未填）
    QDate dischargeDate;    // 出院日期（无效 = 未填）
    quint32 importRecordId = 0; // 所属导入批次（0 = 非导入）
    QDateTime createTime;
};

// 病例查询过滤条件
struct PatientCaseFilter
{
    QString keyword;      // 模糊匹配：病例号/姓名/科室/诊断
    QString department;   // 精确科室
    QDate dateFrom;       // 入院日期起
    QDate dateTo;         // 入院日期止
};
