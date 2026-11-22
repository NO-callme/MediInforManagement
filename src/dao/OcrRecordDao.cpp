// ============================================================================
// 文件功能：mes_ocr_record 表数据访问对象的实现。
// ============================================================================
#include "OcrRecordDao.h"

#include "common/DatabaseManager.h"

namespace {

constexpr const char *kColumns = "id, image_path, recognized_json, status, create_time, operator";

OcrRecord rowToRecord(const DbResult &rs)
{
    OcrRecord r;
    r.id = rs.value(0).toUInt();
    r.imagePath = rs.value(1);
    r.recognizedJson = rs.value(2);
    r.status = rs.value(3);
    r.createTime = QDateTime::fromString(rs.value(4), QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    r.operatorName = rs.value(5);
    return r;
}

} // namespace

bool OcrRecordDao::insert(const OcrRecord &r, quint32 *newId, QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral(
                            "INSERT INTO mes_ocr_record (image_path, recognized_json, status, operator) "
                            "VALUES ('%1', '%2', '%3', '%4')")
                            .arg(db.escape(r.imagePath),
                                 db.escape(r.recognizedJson),
                                 db.escape(r.status),
                                 db.escape(r.operatorName));
    if (!db.execute(sql, nullptr, error))
        return false;
    if (newId)
        *newId = static_cast<quint32>(db.lastInsertId());
    return true;
}

QVector<OcrRecord> OcrRecordDao::findAll(int limit, int offset, QString *error)
{
    QVector<OcrRecord> list;
    DatabaseManager &db = DatabaseManager::instance();
    const QString sql = QStringLiteral("SELECT %1 FROM mes_ocr_record ORDER BY id DESC LIMIT %2 OFFSET %3")
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

int OcrRecordDao::count(QString *error)
{
    DatabaseManager &db = DatabaseManager::instance();
    DbResult rs = db.query(QStringLiteral("SELECT COUNT(*) FROM mes_ocr_record"), error);
    if (!rs.isValid() || !rs.next())
        return -1;
    return rs.value(0).toInt();
}
