// ============================================================================
// 文件功能：mes_patient_case 表的数据访问对象（DAO）声明，提供病例的
//           CRUD、条件筛选与分页查询。
// ============================================================================
#pragma once

#include <QString>
#include <QVector>

#include "model/PatientCase.h"

class PatientCaseDao
{
public:
    bool insert(const PatientCase &c, quint32 *newId = nullptr, QString *error = nullptr);
    bool findById(quint32 id, PatientCase *out = nullptr, QString *error = nullptr);
    QVector<PatientCase> find(const PatientCaseFilter &filter, int limit = 100, int offset = 0,
                              QString *error = nullptr);
    int count(const PatientCaseFilter &filter, QString *error = nullptr);
    bool update(const PatientCase &c, QString *error = nullptr);
    bool remove(quint32 id, QString *error = nullptr);
    int countByCaseNo(const QString &caseNo, QString *error = nullptr); // 导入时查重
};
