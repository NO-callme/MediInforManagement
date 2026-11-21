// ============================================================================
// 文件功能：mes_equipment_test 表数据访问对象的实现。
// ============================================================================
#include "EquipmentTestDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns =
    "id, project_id, equipment_name, model, test_item, test_result, test_date, tester";

QString sqlStr(DatabaseManager &db, const QString &s)
{
    return s.isEmpty() ? QStringLiteral("NULL")
                       : QStringLiteral("'%1'").arg(db.escape(s));
}

QString sqlDate(const QDate &d)
{
    return d.isValid() ? QStringLiteral("'%1'").arg(d.toString(QStringLiteral("yyyy-MM-dd")))
                       : QStringLiteral("NULL");
}

EquipmentTest rowToTest(const DbResult &rs)
{
    EquipmentTest t;
    t.id = rs.value(0).toUInt();
    t.projectId = rs.value(1).toUInt();
    t.equipmentName = rs.value(2);
    t.model = rs.value(3);
    t.testItem = rs.value(4);
    t.testResult = rs.value(5);
    t.testDate = QDate::fromString(rs.value(6), QStringLiteral("yyyy-MM-dd"));
    t.tester = rs.value(7);
    return t;
}

} // namespace

bool EquipmentTestDao::insert(const EquipmentTest &t, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_equipment_test "
                            "(project_id, equipment_name, model, test_item, test_result, test_date, tester) "
                            "VALUES (%1, %2, %3, %4, %5, %6, %7)")
                            .arg(t.projectId)
                            .arg(sqlStr(db, t.equipmentName),
                                 sqlStr(db, t.model),
                                 sqlStr(db, t.testItem),
                                 sqlStr(db, t.testResult),
                                 sqlDate(t.testDate),
                                 sqlStr(db, t.tester));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

QVector<EquipmentTest> EquipmentTestDao::findByProject(quint32 projectId, const QString &keyword, QString *error)
{
    QVector<EquipmentTest> list;
    DatabaseManager &db = DatabaseManager::instance();

    QString where = QStringLiteral(" WHERE project_id = %1").arg(projectId);
    if (!keyword.isEmpty()) {
        const QString kw = db.escape(keyword);
        where += QStringLiteral(
                     " AND (equipment_name LIKE '%%1%' OR model LIKE '%%1%' "
                     "OR test_item LIKE '%%1%' OR tester LIKE '%%1%')")
                     .arg(kw);
    }

    const QString sql = QStringLiteral("SELECT %1 FROM mes_equipment_test%2 ORDER BY test_date DESC, id")
                            .arg(kColumns, where);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return list;
    while (rs.next())
        list.append(rowToTest(rs));
    return list;
}

bool EquipmentTestDao::update(const EquipmentTest &t, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "UPDATE mes_equipment_test SET equipment_name=%1, model=%2, test_item=%3, "
                            "test_result=%4, test_date=%5, tester=%6 WHERE id=%7")
                            .arg(sqlStr(db, t.equipmentName),
                                 sqlStr(db, t.model),
                                 sqlStr(db, t.testItem),
                                 sqlStr(db, t.testResult),
                                 sqlDate(t.testDate),
                                 sqlStr(db, t.tester),
                                 QString::number(t.id));
    return db.execute(sql, nullptr, error);
}

bool EquipmentTestDao::remove(quint32 id, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    return db.execute(QStringLiteral("DELETE FROM mes_equipment_test WHERE id = %1").arg(id), nullptr, error);
}

void EquipmentTestDao::removeByProject(quint32 projectId)
{
    DatabaseManager &db = DatabaseManager::instance();
    db.execute(QStringLiteral("DELETE FROM mes_equipment_test WHERE project_id = %1").arg(projectId));
}
