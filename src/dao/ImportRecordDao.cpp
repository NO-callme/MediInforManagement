// ============================================================================
// 文件功能：mes_import_record 表数据访问对象的实现。
// ============================================================================
#include "ImportRecordDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns =
    "id, file_name, total_rows, success_rows, failed_rows, import_time, operator";

ImportRecord rowToRecord(const DbResult &rs)
{
    ImportRecord r;
    r.id = rs.value(0).toUInt();
    r.fileName = rs.value(1);
    r.totalRows = rs.value(2).toInt();
    r.successRows = rs.value(3).toInt();
    r.failedRows = rs.value(4).toInt();
    r.importTime = QDateTime::fromString(rs.value(5), QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    r.operatorName = rs.value(6);
    return r;
}

} // namespace

bool ImportRecordDao::insert(ImportRecord &r, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_import_record "
                            "(file_name, total_rows, success_rows, failed_rows, import_time, operator) "
                            "VALUES ('%1', %2, %3, %4, NOW(), '%5')")
                            .arg(db.escape(r.fileName))
                            .arg(r.totalRows)
                            .arg(r.successRows)
                            .arg(r.failedRows)
                            .arg(db.escape(r.operatorName));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    r.id = static_cast<quint32>(db.lastInsertId());
    return true;
}

bool ImportRecordDao::findById(quint32 id, ImportRecord *out, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_import_record WHERE id = %2")
                            .arg(kColumns)
                            .arg(id);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return false;
    if (!rs.next())
        return false;
    if (out)
        *out = rowToRecord(rs);
    return true;
}

QVector<ImportRecord> ImportRecordDao::findAll(int limit, int offset, QString *error)
{
    QVector<ImportRecord> list;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_import_record ORDER BY id DESC LIMIT %2 OFFSET %3")
                            .arg(kColumns)
                            .arg(limit)
                            .arg(offset);
    DbResult rs = db.query(sql, error);
    if (!rs.isValid())
        return list;
    while (rs.next())
        list.append(rowToRecord(rs));
    return list;
}

int ImportRecordDao::count(QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    DbResult rs = db.query(QStringLiteral("SELECT COUNT(*) FROM mes_import_record"), error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}
