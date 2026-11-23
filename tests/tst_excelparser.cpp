// ============================================================================
// 文件功能：Excel 病例解析器单元测试。验证文件缺失处理与逐行校验逻辑。
// ============================================================================
#include <QtTest>

#include <QFile>

#include "excel/ExcelCaseParser.h"
#include "xlsxdocument.h"

class TestExcelParser : public QObject
{
    Q_OBJECT

private slots:
    void testMissingFile();
    void testParseValidAndInvalid();
};

void TestExcelParser::testMissingFile()
{
    const ExcelParseResult r = ExcelCaseParser::parse(QStringLiteral("__nonexistent__.xlsx"));
    QVERIFY(!r.ok);
    QVERIFY(!r.error.isEmpty());
}

void TestExcelParser::testParseValidAndInvalid()
{
    const QString path = QStringLiteral("__test_cases.xlsx");
    {
        QXlsx::Document doc;
        doc.write(1, 1, QStringLiteral("病例号"));
        doc.write(1, 2, QStringLiteral("姓名"));
        doc.write(1, 3, QStringLiteral("性别"));
        doc.write(1, 4, QStringLiteral("年龄"));
        doc.write(1, 5, QStringLiteral("科室"));
        doc.write(1, 6, QStringLiteral("诊断"));
        doc.write(1, 7, QStringLiteral("入院日期"));
        doc.write(1, 8, QStringLiteral("出院日期"));
        // 有效行
        doc.write(2, 1, QStringLiteral("T001"));
        doc.write(2, 2, QStringLiteral("张三"));
        doc.write(2, 4, 30);
        doc.write(2, 7, QStringLiteral("2026-01-01"));
        // 非法行：年龄不是整数
        doc.write(3, 1, QStringLiteral("T002"));
        doc.write(3, 2, QStringLiteral("李四"));
        doc.write(3, 4, QStringLiteral("abc"));
        doc.saveAs(path);
    }

    const ExcelParseResult r = ExcelCaseParser::parse(path);
    QVERIFY(r.ok);
    QCOMPARE(r.rows.size(), 2);
    QCOMPARE(r.validCount(), 1);
    QCOMPARE(r.invalidCount(), 1);
    QCOMPARE(r.rows.at(0).data.caseNo, QStringLiteral("T001"));
    QVERIFY(r.rows.at(0).error.isEmpty());
    QVERIFY(!r.rows.at(1).error.isEmpty());

    QFile::remove(path);
}

QTEST_GUILESS_MAIN(TestExcelParser)
#include "tst_excelparser.moc"
