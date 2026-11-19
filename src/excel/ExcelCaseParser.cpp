// ============================================================================
// 文件功能：Excel 病例数据解析器实现。读取第一个工作表，从第 2 行起按 8 列
//           模板解析，校验必填项、年龄整数、日期格式等。
// ============================================================================
#include "ExcelCaseParser.h"

#include <QDate>
#include <QFileInfo>
#include <QStringList>

#include "xlsxdocument.h"

namespace {

QString cellStr(const QXlsx::Document &doc, int row, int col)
{
    const QVariant v = doc.read(row, col);
    if (v.isNull() || !v.isValid())
        return QString();
    return v.toString().trimmed();
}

// 兼容常见日期格式；无法解析返回无效 QDate
QDate parseDate(const QString &s)
{
    if (s.isEmpty())
        return QDate();
    const char *fmts[] = {
        "yyyy-MM-dd", "yyyy/M/d", "yyyy/M/dd", "yyyy/MM/d", "yyyy/MM/dd",
        "yyyy-M-d", "yyyy.MM.dd", "yyyy年M月d日"
    };
    for (const char *f : fmts) {
        const QDate d = QDate::fromString(s, QLatin1String(f));
        if (d.isValid())
            return d;
    }
    return QDate();
}

// 解析整数：空串 -> -1（未填）；非法 -> false
bool parseInt(const QString &s, int *out)
{
    if (s.isEmpty()) {
        *out = -1;
        return true;
    }
    bool ok = false;
    const int v = s.toInt(&ok);
    if (!ok)
        return false;
    *out = v;
    return true;
}

} // namespace

int ExcelParseResult::validCount() const
{
    int n = 0;
    for (const ParsedCaseRow &r : rows)
        if (r.error.isEmpty())
            ++n;
    return n;
}

int ExcelParseResult::invalidCount() const
{
    int n = 0;
    for (const ParsedCaseRow &r : rows)
        if (!r.error.isEmpty())
            ++n;
    return n;
}

ExcelParseResult ExcelCaseParser::parse(const QString &filePath)
{
    ExcelParseResult result;

    if (!QFileInfo::exists(filePath)) {
        result.error = QStringLiteral("文件不存在: %1").arg(filePath);
        return result;
    }

    QXlsx::Document doc(filePath);
    if (!doc.load()) {
        result.error = QStringLiteral("无法打开 Excel 文件（可能不是有效的 .xlsx）");
        return result;
    }

    const int lastRow = doc.dimension().lastRow();
    if (lastRow < 2) {
        result.ok = true;
        result.error = QStringLiteral("文件中没有数据行");
        return result;
    }

    result.ok = true;
    for (int row = 2; row <= lastRow; ++row) {
        const QString caseNo = cellStr(doc, row, 1);
        const QString name = cellStr(doc, row, 2);
        const QString gender = cellStr(doc, row, 3);
        const QString ageStr = cellStr(doc, row, 4);
        const QString dept = cellStr(doc, row, 5);
        const QString diag = cellStr(doc, row, 6);
        const QString admitStr = cellStr(doc, row, 7);
        const QString dischargeStr = cellStr(doc, row, 8);

        // 整行全空则跳过（不计入结果）
        if (caseNo.isEmpty() && name.isEmpty() && gender.isEmpty() && ageStr.isEmpty()
            && dept.isEmpty() && diag.isEmpty() && admitStr.isEmpty() && dischargeStr.isEmpty()) {
            continue;
        }

        QStringList errs;
        if (caseNo.isEmpty())
            errs << QStringLiteral("病例号不能为空");

        int age = -1;
        if (!parseInt(ageStr, &age))
            errs << QStringLiteral("年龄必须是整数");
        else if (age > 150)
            errs << QStringLiteral("年龄超出合理范围");

        const QDate admit = parseDate(admitStr);
        if (!admitStr.isEmpty() && !admit.isValid())
            errs << QStringLiteral("入院日期格式错误");

        const QDate discharge = parseDate(dischargeStr);
        if (!dischargeStr.isEmpty() && !discharge.isValid())
            errs << QStringLiteral("出院日期格式错误");

        if (admit.isValid() && discharge.isValid() && discharge < admit)
            errs << QStringLiteral("出院日期早于入院日期");

        ParsedCaseRow parsed;
        parsed.rowNumber = row;
        parsed.data.caseNo = caseNo;
        parsed.data.patientName = name;
        parsed.data.gender = gender;
        parsed.data.age = age;
        parsed.data.department = dept;
        parsed.data.diagnosis = diag;
        parsed.data.admitDate = admit;
        parsed.data.dischargeDate = discharge;
        parsed.error = errs.join(QStringLiteral("；"));

        result.rows.append(parsed);
    }
    return result;
}
