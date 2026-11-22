// ============================================================================
// 文件功能：报表统计 DAO 实现，基于 DatabaseManager 执行聚合 SQL。
// ============================================================================
#include "ReportDao.h"

#include "common/DatabaseManager.h"

QVector<StatItem> ReportDao::patientCasesByDepartment(QString *error)
{
    QVector<StatItem> result;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
        "SELECT IF(department IS NULL OR department='', '未填', department) AS d, COUNT(*) AS c "
        "FROM mes_patient_case GROUP BY d ORDER BY c DESC");
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return result;
    while (rs.next())
        result.append({rs.value(0), rs.value(1).toDouble()});
    return result;
}

QVector<StatItem> ReportDao::patientCasesByMonth(QString *error)
{
    QVector<StatItem> result;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
        "SELECT DATE_FORMAT(admit_date, '%Y-%m') AS ym, COUNT(*) AS c "
        "FROM mes_patient_case WHERE admit_date IS NOT NULL GROUP BY ym ORDER BY ym");
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return result;
    while (rs.next())
        result.append({rs.value(0), rs.value(1).toDouble()});
    return result;
}

QVector<StatItem> ReportDao::projectStatusDistribution(QString *error)
{
    QVector<StatItem> result;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
        "SELECT status, COUNT(*) AS c FROM mes_project GROUP BY status ORDER BY c DESC");
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return result;
    while (rs.next())
        result.append({rs.value(0), rs.value(1).toDouble()});
    return result;
}

QVector<StatItem> ReportDao::testPassRateByMonth(QString *error)
{
    QVector<StatItem> result;
    DatabaseManager &db = DatabaseManager::instance();
    // 通过判定：结果含 通过/合格/成功/正常
    const QString sql = QStringLiteral(
        "SELECT DATE_FORMAT(test_date, '%Y-%m') AS ym, "
        "SUM(CASE WHEN test_result LIKE '%通过%' OR test_result LIKE '%合格%' "
        "OR test_result LIKE '%成功%' OR test_result LIKE '%正常%' THEN 1 ELSE 0 END) AS pass_cnt, "
        "COUNT(*) AS total "
        "FROM mes_equipment_test WHERE test_date IS NOT NULL GROUP BY ym ORDER BY ym");
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return result;
    while (rs.next()) {
        const double total = rs.value(2).toDouble();
        const double rate = total > 0 ? rs.value(1).toDouble() / total * 100.0 : 0.0;
        result.append({rs.value(0), rate});
    }
    return result;
}
