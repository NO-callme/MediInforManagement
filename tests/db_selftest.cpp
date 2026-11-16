// ============================================================================
// 文件功能：数据层自测程序。验证 DatabaseManager 连接 + UserDao 完整 CRUD
//           （插入/查询/更新/分页/删除），任一环节失败返回非 0 退出码。
// ============================================================================
#include <QCoreApplication>
#include <QDebug>

#include "common/DatabaseManager.h"
#include "dao/UserDao.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString configPath = QStringLiteral("config.ini");
    if (argc > 1)
        configPath = QString::fromLocal8Bit(argv[1]);

    DatabaseManager &db = DatabaseManager::instance();
    if (!db.loadConfig(configPath)) {
        qCritical() << "加载配置失败:" << configPath;
        return 1;
    }

    QString err;
    if (!db.connect(&err)) {
        qCritical() << "连接数据库失败:" << err;
        return 1;
    }
    qInfo() << "连接成功:" << db.host() << ":" << db.port() << "/" << db.database();

    UserDao dao;

    // 1. 查询总数
    int before = dao.count(&err);
    qInfo() << "[count] 当前用户数 =" << before;

    // 2. 插入
    User u;
    u.username = QStringLiteral("test_user");
    u.password = QStringLiteral("123456");
    u.role = QStringLiteral("operator");
    u.realName = QStringLiteral("测试用户");
    quint32 newId = 0;
    if (!dao.insert(u, &newId, &err)) {
        qCritical() << "[insert] 失败:" << err;
        return 1;
    }
    qInfo() << "[insert] 成功, 新 id =" << newId;

    // 3. 按用户名查询
    User found;
    if (!dao.findByUsername(QStringLiteral("test_user"), &found, &err)) {
        qCritical() << "[findByUsername] 失败:" << err;
        return 1;
    }
    qInfo() << "[findByUsername] 成功:" << found.id << found.username << found.role << found.realName;

    // 4. 更新
    found.realName = QStringLiteral("测试用户-已修改");
    found.role = QStringLiteral("admin");
    if (!dao.update(found, &err)) {
        qCritical() << "[update] 失败:" << err;
        return 1;
    }
    User afterUpdate;
    dao.findById(found.id, &afterUpdate, &err);
    qInfo() << "[update] 后:" << afterUpdate.id << afterUpdate.username
            << afterUpdate.role << afterUpdate.realName;

    // 5. 分页查询
    const QVector<User> all = dao.findAll(10, 0, &err);
    qInfo() << "[findAll] 返回" << all.size() << "条:";
    for (const User &item : all)
        qInfo() << "    " << item.id << item.username << item.role << item.realName;

    // 6. 删除
    if (!dao.remove(found.id, &err)) {
        qCritical() << "[remove] 失败:" << err;
        return 1;
    }
    int after = dao.count(&err);
    qInfo() << "[remove] 成功, 当前用户数 =" << after << "(删除前" << before << ")";

    qInfo() << "==== DB 自测全部通过 ====";
    return 0;
}
