// ============================================================================
// 文件功能：查询结果集 DbResult 的 RAII 封装声明，自动释放 MYSQL_RES，
//           提供逐行迭代与列取值接口。
// ============================================================================
#pragma once

#include <QString>

#include <winsock2.h>
#include <mysql.h>

// RAII 封装 MYSQL_RES，负责自动释放结果集
class DbResult
{
public:
    DbResult() = default;
    explicit DbResult(MYSQL_RES *res);
    ~DbResult();

    DbResult(const DbResult &) = delete;
    DbResult &operator=(const DbResult &) = delete;

    DbResult(DbResult &&other) noexcept;
    DbResult &operator=(DbResult &&other) noexcept;

    bool isValid() const;
    bool next();                    // 前进到下一行；无更多行返回 false
    quint64 rowCount() const;
    int columnCount() const;
    QString value(int column) const;       // 当前行第 column 列（0 起）
    bool isNull(int column) const;
    qlonglong toLongLong(int column, bool *ok = nullptr) const;

private:
    void release();

    MYSQL_RES *res_ = nullptr;
    MYSQL_ROW row_ = nullptr;
    unsigned int fieldCount_ = 0;
};
