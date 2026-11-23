// ============================================================================
// 文件功能：mes_patient_case 表数据访问对象的实现，SQL 经 DatabaseManager 执行。
// ============================================================================
#include "PatientCaseDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns =
    "id, case_no, patient_name, gender, age, department, diagnosis, "
    "admit_date, discharge_date, import_record_id, create_time";

// 字符串列：空串 -> NULL，否则转义后加引号
QString sqlStr(DatabaseManager &db, const QString &s)
{
    return s.isEmpty() ? QStringLiteral("NULL")
                       : QStringLiteral("'%1'").arg(db.escape(s));
}

// 日期列：无效日期 -> NULL，否则 'yyyy-MM-dd'
QString sqlDate(const QDate &d)
{
    return d.isValid() ? QStringLiteral("'%1'").arg(d.toString(QStringLiteral("yyyy-MM-dd")))
                       : QStringLiteral("NULL");
}

QString buildWhere(const PatientCaseFilter &f)
{
    DatabaseManager &db = DatabaseManager::instance();
    QStringList conds;
    if (!f.keyword.isEmpty()) {
        const QString kw = db.escape(f.keyword);
        conds << QStringLiteral(
                     "(case_no LIKE '%%1%' OR patient_name LIKE '%%1%' "
                     "OR department LIKE '%%1%' OR diagnosis LIKE '%%1%')")
                     .arg(kw);
    }
    if (!f.department.isEmpty())
        conds << QStringLiteral("department = '%1'").arg(db.escape(f.department));
    if (f.dateFrom.isValid())
        conds << QStringLiteral("admit_date >= '%1'")
                     .arg(f.dateFrom.toString(QStringLiteral("yyyy-MM-dd")));
    if (f.dateTo.isValid())
        conds << QStringLiteral("admit_date <= '%1'")
                     .arg(f.dateTo.toString(QStringLiteral("yyyy-MM-dd")));

    return conds.isEmpty() ? QString()
                           : QStringLiteral(" WHERE ") + conds.join(QStringLiteral(" AND "));
}

PatientCase rowToCase(const DbResult &rs)
{
    PatientCase c;
    c.id = rs.value(0).toUInt();
    c.caseNo = rs.value(1);
    c.patientName = rs.value(2);
    c.gender = rs.value(3);
    c.age = rs.isNull(4) ? -1 : rs.value(4).toInt();
    c.department = rs.value(5);
    c.diagnosis = rs.value(6);
    c.admitDate = QDate::fromString(rs.value(7), QStringLiteral("yyyy-MM-dd"));
    c.dischargeDate = QDate::fromString(rs.value(8), QStringLiteral("yyyy-MM-dd"));
    c.importRecordId = rs.isNull(9) ? 0 : rs.value(9).toUInt();
    c.createTime = QDateTime::fromString(rs.value(10), QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    return c;
}

} // namespace

bool PatientCaseDao::insert(const PatientCase &c, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_patient_case "
                            "(case_no, patient_name, gender, age, department, diagnosis, "
                            " admit_date, discharge_date, import_record_id) "
                            "VALUES ('%1', %2, %3, %4, %5, %6, %7, %8, %9)")
                            .arg(db.escape(c.caseNo),
                                 sqlStr(db, c.patientName),
                                 sqlStr(db, c.gender),
                                 c.age >= 0 ? QString::number(c.age) : QStringLiteral("NULL"),
                                 sqlStr(db, c.department),
                                 sqlStr(db, c.diagnosis),
                                 sqlDate(c.admitDate),
                                 sqlDate(c.dischargeDate),
                                 c.importRecordId > 0 ? QString::number(c.importRecordId)
                                                      : QStringLiteral("NULL"));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

bool PatientCaseDao::findById(quint32 id, PatientCase *out, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_patient_case WHERE id = %2")
                            .arg(kColumns)
                            .arg(id);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return false;
    if (!rs.next())
        return false;
    if (out)
        *out = rowToCase(rs);
    return true;
}

QVector<PatientCase> PatientCaseDao::find(const PatientCaseFilter &filter, int limit, int offset,
                                          QString *error)
{
    QVector<PatientCase> list;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_patient_case%2 ORDER BY id DESC LIMIT %3 OFFSET %4")
                            .arg(kColumns, buildWhere(filter))
                            .arg(limit)
                            .arg(offset);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return list;
    while (rs.next())
        list.append(rowToCase(rs));
    return list;
}

int PatientCaseDao::count(const PatientCaseFilter &filter, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT COUNT(*) FROM mes_patient_case%1")
                            .arg(buildWhere(filter));
    DbResult rs = db.query(sql, error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}

bool PatientCaseDao::update(const PatientCase &c, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "UPDATE mes_patient_case SET "
                            "case_no='%1', patient_name=%2, gender=%3, age=%4, department=%5, "
                            "diagnosis=%6, admit_date=%7, discharge_date=%8 "
                            "WHERE id=%9")
                            .arg(db.escape(c.caseNo),
                                 sqlStr(db, c.patientName),
                                 sqlStr(db, c.gender),
                                 c.age >= 0 ? QString::number(c.age) : QStringLiteral("NULL"),
                                 sqlStr(db, c.department),
                                 sqlStr(db, c.diagnosis),
                                 sqlDate(c.admitDate),
                                 sqlDate(c.dischargeDate))
                            .arg(c.id);
    return db.execute(sql, nullptr, error);
}

bool PatientCaseDao::remove(quint32 id, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("DELETE FROM mes_patient_case WHERE id = %1").arg(id);
    return db.execute(sql, nullptr, error);
}

int PatientCaseDao::countByCaseNo(const QString &caseNo, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT COUNT(*) FROM mes_patient_case WHERE case_no = '%1'")
                            .arg(db.escape(caseNo));
    DbResult rs = db.query(sql, error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}

QSet<QString> PatientCaseDao::findExistingCaseNos(const QStringList &caseNos, QString *error)
{
    QSet<QString> existing;
    DatabaseManager &db = DatabaseManager::instance();

    // 分批查询，避免 IN 列表过长
    constexpr int kChunkSize = 500;
    for (int i = 0; i < caseNos.size(); i += kChunkSize) {
        QStringList quoted;
        const int end = qMin(i + kChunkSize, caseNos.size());
        for (int j = i; j < end; ++j)
            quoted << QStringLiteral("'%1'").arg(db.escape(caseNos.at(j)));

        const QString sql = QStringLiteral("SELECT case_no FROM mes_patient_case WHERE case_no IN (%1)")
                                .arg(quoted.join(QStringLiteral(",")));
        DbResult rs = db.query(sql, error);
        if (!rs.isValid())
            return existing;
        while (rs.next())
            existing.insert(rs.value(0));
    }
    return existing;
}
