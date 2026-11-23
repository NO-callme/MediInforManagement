// ============================================================================
// 文件功能：mes_equipment 表数据访问对象的实现。
// ============================================================================
#include "EquipmentDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns =
    "id, equipment_no, name, model, category, status, purchase_date, department, remark, create_time";

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

QString buildWhere(const EquipmentFilter &f)
{
    DatabaseManager &db = DatabaseManager::instance();
    QStringList conds;
    if (!f.keyword.isEmpty()) {
        const QString kw = db.escape(f.keyword);
        conds << QStringLiteral(
                     "(equipment_no LIKE '%%1%' OR name LIKE '%%1%' "
                     "OR model LIKE '%%1%' OR department LIKE '%%1%')")
                     .arg(kw);
    }
    if (!f.status.isEmpty())
        conds << QStringLiteral("status = '%1'").arg(db.escape(f.status));
    if (!f.category.isEmpty())
        conds << QStringLiteral("category = '%1'").arg(db.escape(f.category));
    return conds.isEmpty() ? QString()
                           : QStringLiteral(" WHERE ") + conds.join(QStringLiteral(" AND "));
}

Equipment rowToEquipment(const DbResult &rs)
{
    Equipment e;
    e.id = rs.value(0).toUInt();
    e.equipmentNo = rs.value(1);
    e.name = rs.value(2);
    e.model = rs.value(3);
    e.category = rs.value(4);
    e.status = rs.value(5);
    e.purchaseDate = QDate::fromString(rs.value(6), QStringLiteral("yyyy-MM-dd"));
    e.department = rs.value(7);
    e.remark = rs.value(8);
    e.createTime = QDateTime::fromString(rs.value(9), QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    return e;
}

} // namespace

bool EquipmentDao::insert(const Equipment &e, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_equipment "
                            "(equipment_no, name, model, category, status, purchase_date, department, remark) "
                            "VALUES ('%1', '%2', %3, %4, '%5', %6, %7, %8)")
                            .arg(db.escape(e.equipmentNo),
                                 db.escape(e.name),
                                 sqlStr(db, e.model),
                                 sqlStr(db, e.category),
                                 db.escape(e.status),
                                 sqlDate(e.purchaseDate),
                                 sqlStr(db, e.department),
                                 sqlStr(db, e.remark));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

bool EquipmentDao::findById(quint32 id, Equipment *out, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_equipment WHERE id = %2")
                            .arg(kColumns)
                            .arg(id);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return false;
    if (!rs.next())
        return false;
    if (out)
        *out = rowToEquipment(rs);
    return true;
}

QVector<Equipment> EquipmentDao::find(const EquipmentFilter &filter, int limit, int offset, QString *error)
{
    QVector<Equipment> list;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_equipment%2 ORDER BY id DESC LIMIT %3 OFFSET %4")
                            .arg(kColumns, buildWhere(filter))
                            .arg(limit)
                            .arg(offset);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return list;
    while (rs.next())
        list.append(rowToEquipment(rs));
    return list;
}

int EquipmentDao::count(const EquipmentFilter &filter, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT COUNT(*) FROM mes_equipment%1").arg(buildWhere(filter));
    DbResult rs = db.query(sql, error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}

bool EquipmentDao::update(const Equipment &e, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "UPDATE mes_equipment SET equipment_no='%1', name='%2', model=%3, category=%4, "
                            "status='%5', purchase_date=%6, department=%7, remark=%8 WHERE id=%9")
                            .arg(db.escape(e.equipmentNo),
                                 db.escape(e.name),
                                 sqlStr(db, e.model),
                                 sqlStr(db, e.category),
                                 db.escape(e.status),
                                 sqlDate(e.purchaseDate),
                                 sqlStr(db, e.department),
                                 sqlStr(db, e.remark),
                                 QString::number(e.id));
    return db.execute(sql, nullptr, error);
}

bool EquipmentDao::remove(quint32 id, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    return db.execute(QStringLiteral("DELETE FROM mes_equipment WHERE id = %1").arg(id), nullptr, error);
}

int EquipmentDao::countByNo(const QString &equipmentNo, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT COUNT(*) FROM mes_equipment WHERE equipment_no = '%1'")
                            .arg(db.escape(equipmentNo));
    DbResult rs = db.query(sql, error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}
