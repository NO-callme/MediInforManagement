#include "DbResult.h"

DbResult::DbResult(MYSQL_RES *res)
    : res_(res)
{
    if (res_)
        fieldCount_ = mysql_num_fields(res_);
}

DbResult::~DbResult()
{
    release();
}

DbResult::DbResult(DbResult &&other) noexcept
    : res_(other.res_)
    , row_(other.row_)
    , fieldCount_(other.fieldCount_)
{
    other.res_ = nullptr;
    other.row_ = nullptr;
    other.fieldCount_ = 0;
}

DbResult &DbResult::operator=(DbResult &&other) noexcept
{
    if (this != &other) {
        release();
        res_ = other.res_;
        row_ = other.row_;
        fieldCount_ = other.fieldCount_;
        other.res_ = nullptr;
        other.row_ = nullptr;
        other.fieldCount_ = 0;
    }
    return *this;
}

void DbResult::release()
{
    if (res_) {
        mysql_free_result(res_);
        res_ = nullptr;
    }
    row_ = nullptr;
    fieldCount_ = 0;
}

bool DbResult::isValid() const
{
    return res_ != nullptr;
}

bool DbResult::next()
{
    if (!res_)
        return false;
    row_ = mysql_fetch_row(res_);
    return row_ != nullptr;
}

quint64 DbResult::rowCount() const
{
    return res_ ? mysql_num_rows(res_) : 0;
}

int DbResult::columnCount() const
{
    return res_ ? static_cast<int>(fieldCount_) : 0;
}

QString DbResult::value(int column) const
{
    if (!res_ || !row_ || column < 0 || column >= static_cast<int>(fieldCount_))
        return QString();
    const char *v = row_[column];
    return v ? QString::fromUtf8(v) : QString();
}

bool DbResult::isNull(int column) const
{
    if (!res_ || !row_ || column < 0 || column >= static_cast<int>(fieldCount_))
        return true;
    return row_[column] == nullptr;
}

qlonglong DbResult::toLongLong(int column, bool *ok) const
{
    return value(column).toLongLong(ok);
}
