// ============================================================================
// 文件功能：模拟 OCR 引擎单元测试。验证成功/失败/文件缺失三种情形。
// ============================================================================
#include <QtTest>

#include <QFile>

#include "ocr/MockOcrEngine.h"

class TestOcrEngine : public QObject
{
    Q_OBJECT

private slots:
    void testMissingFile();
    void testSuccess();
    void testFail();
};

void TestOcrEngine::testMissingFile()
{
    MockOcrEngine e;
    const OcrResult r = e.recognize(QStringLiteral("__nonexistent__.png"));
    QVERIFY(!r.ok);
    QVERIFY(!r.error.isEmpty());
}

void TestOcrEngine::testSuccess()
{
    QFile f(QStringLiteral("__test_ocr.png"));
    f.open(QIODevice::WriteOnly);
    f.write("x");
    f.close();

    MockOcrEngine e;
    const OcrResult r = e.recognize(QStringLiteral("__test_ocr.png"));
    QVERIFY(r.ok);
    QVERIFY(r.fields.contains(QStringLiteral("病例号")));
    QVERIFY(r.fields.contains(QStringLiteral("姓名")));

    QFile::remove(QStringLiteral("__test_ocr.png"));
}

void TestOcrEngine::testFail()
{
    QFile f(QStringLiteral("__test-fail.png"));
    f.open(QIODevice::WriteOnly);
    f.write("x");
    f.close();

    MockOcrEngine e;
    const OcrResult r = e.recognize(QStringLiteral("__test-fail.png"));
    QVERIFY(!r.ok);
    QVERIFY(!r.error.isEmpty());

    QFile::remove(QStringLiteral("__test-fail.png"));
}

QTEST_GUILESS_MAIN(TestOcrEngine)
#include "tst_ocr.moc"
