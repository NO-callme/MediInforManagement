// ============================================================================
// 文件功能：DAO 集成测试。验证项目/里程碑 CRUD 与报表聚合查询（需数据库）。
// ============================================================================
#include <QtTest>

#include <QFileInfo>

#include "common/DatabaseManager.h"
#include "dao/MilestoneDao.h"
#include "dao/ProjectDao.h"
#include "dao/ReportDao.h"

class TestDao : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testProjectCrud();
    void testMilestoneCrud();
    void testReportQueries();
};

void TestDao::initTestCase()
{
    QString configPath;
    for (const QString &c : {QStringLiteral("config.ini"),
                             QStringLiteral("../config.ini"),
                             QStringLiteral("../../config.ini")}) {
        if (QFileInfo::exists(c)) {
            configPath = c;
            break;
        }
    }
    if (configPath.isEmpty())
        QSKIP("未找到 config.ini，跳过数据库测试");

    DatabaseManager &db = DatabaseManager::instance();
    if (!db.loadConfig(configPath))
        QSKIP("加载配置失败，跳过数据库测试");
    if (!db.connect())
        QSKIP("无法连接数据库，跳过数据库测试");
}

void TestDao::cleanupTestCase()
{
    DatabaseManager::instance().close();
}

void TestDao::testProjectCrud()
{
    ProjectDao dao;

    Project p;
    p.projectNo = QStringLiteral("TEST-P7-001");
    p.name = QStringLiteral("P7测试项目");
    p.status = QStringLiteral("进行中");
    p.owner = QStringLiteral("测试员");
    p.progress = 10;

    quint32 id = 0;
    QVERIFY(dao.insert(p, &id));
    QVERIFY(id > 0);

    Project out;
    QVERIFY(dao.findById(id, &out));
    QCOMPARE(out.name, QStringLiteral("P7测试项目"));

    out.progress = 50;
    QVERIFY(dao.update(out));

    QVERIFY(dao.remove(id));
}

void TestDao::testMilestoneCrud()
{
    ProjectDao pdao;
    Project p;
    p.projectNo = QStringLiteral("TEST-P7-002");
    p.name = QStringLiteral("P7里程碑测试");
    p.status = QStringLiteral("进行中");
    quint32 pid = 0;
    QVERIFY(pdao.insert(p, &pid));

    MilestoneDao mdao;
    Milestone m;
    m.projectId = pid;
    m.name = QStringLiteral("里程碑1");
    m.planDate = QDate::currentDate().addDays(-10); // 已过期
    m.status = QStringLiteral("未开始");
    QVERIFY(mdao.insert(m));

    const QVector<Milestone> list = mdao.findByProject(pid);
    QVERIFY(list.size() >= 1);

    mdao.removeByProject(pid);
    pdao.remove(pid);
}

void TestDao::testReportQueries()
{
    ReportDao dao;
    QString err;

    const QVector<StatItem> byDept = dao.patientCasesByDepartment(&err);
    QVERIFY(err.isEmpty());

    const QVector<StatItem> byMonth = dao.patientCasesByMonth(&err);
    QVERIFY(err.isEmpty());

    const QVector<StatItem> status = dao.projectStatusDistribution(&err);
    QVERIFY(err.isEmpty());

    const QVector<StatItem> passRate = dao.testPassRateByMonth(&err);
    QVERIFY(err.isEmpty());
}

QTEST_GUILESS_MAIN(TestDao)
#include "tst_dao.moc"
