// ============================================================================
// 文件功能：示例病例数据生成工具。用 QXlsx 写出一份 20 条虚构病例的
//           .xlsx（均为虚构数据，不涉及真实个人隐私），供导入功能测试使用。
//           用法：gen_sample_xlsx [输出路径]（默认 resources/病例数据.xlsx）
// ============================================================================
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "xlsxdocument.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString outPath = QStringLiteral("resources/病例数据.xlsx");
    if (argc > 1)
        outPath = QString::fromLocal8Bit(argv[1]);

    struct Row {
        const char *no;
        const char *name;
        const char *gender;
        int age;
        const char *dept;
        const char *diag;
        const char *admit;
        const char *discharge;
    };

    // 20 条虚构病例数据
    const Row rows[] = {
        {"2026001", "张伟", "男", 45, "心内科",   "高血压",               "2026-01-05", "2026-01-12"},
        {"2026002", "李娜", "女", 32, "呼吸内科", "肺炎",                 "2026-01-08", "2026-01-20"},
        {"2026003", "王芳", "女", 58, "消化内科", "慢性胃炎",             "2026-01-11", "2026-01-18"},
        {"2026004", "刘洋", "男", 27, "神经内科", "偏头痛",               "2026-02-02", "2026-02-06"},
        {"2026005", "陈静", "女", 41, "骨科",     "腰椎间盘突出",         "2026-02-10", "2026-02-22"},
        {"2026006", "赵磊", "男", 33, "普外科",   "急性阑尾炎",           "2026-02-15", "2026-02-21"},
        {"2026007", "孙敏", "女", 6,  "儿科",     "支气管炎",             "2026-02-18", "2026-02-25"},
        {"2026008", "周杰", "男", 50, "内分泌科", "2型糖尿病",            "2026-03-01", "2026-03-09"},
        {"2026009", "吴霞", "女", 29, "妇产科",   "妊娠高血压",           "2026-03-05", "2026-03-12"},
        {"2026010", "郑强", "男", 62, "心内科",   "冠心病",               "2026-03-10", "2026-03-24"},
        {"2026011", "冯丽", "女", 37, "呼吸内科", "支气管哮喘",           "2026-03-14", "2026-03-19"},
        {"2026012", "许涛", "男", 44, "消化内科", "胃溃疡",               "2026-04-02", "2026-04-11"},
        {"2026013", "何静", "女", 55, "神经内科", "脑供血不足",           "2026-04-07", "2026-04-15"},
        {"2026014", "邓超", "男", 30, "骨科",     "桡骨骨折",             "2026-04-12", "2026-04-26"},
        {"2026015", "蒋丽", "女", 48, "内分泌科", "甲状腺功能亢进",       "2026-05-01", "2026-05-10"},
        {"2026016", "沈飞", "男", 39, "泌尿外科", "肾结石",               "2026-05-06", "2026-05-13"},
        {"2026017", "韩雪", "女", 24, "皮肤科",   "湿疹",                 "2026-05-11", "2026-05-16"},
        {"2026018", "秦明", "男", 67, "心内科",   "心力衰竭",             "2026-06-03", "2026-06-20"},
        {"2026019", "杨颖", "女", 31, "眼科",     "急性结膜炎",           "2026-06-08", "2026-06-11"},
        {"2026020", "曹勇", "男", 52, "呼吸内科", "慢性阻塞性肺疾病",     "2026-06-15", "2026-06-28"},
    };
    constexpr int kCount = sizeof(rows) / sizeof(rows[0]);

    using namespace QXlsx;
    Document xlsx;

    const QStringList headers = {
        QStringLiteral("病例号"), QStringLiteral("姓名"), QStringLiteral("性别"),
        QStringLiteral("年龄"), QStringLiteral("科室"), QStringLiteral("诊断"),
        QStringLiteral("入院日期"), QStringLiteral("出院日期")
    };
    for (int c = 0; c < headers.size(); ++c)
        xlsx.write(1, c + 1, headers[c]);

    for (int i = 0; i < kCount; ++i) {
        const Row &r = rows[i];
        xlsx.write(i + 2, 1, QString::fromUtf8(r.no));
        xlsx.write(i + 2, 2, QString::fromUtf8(r.name));
        xlsx.write(i + 2, 3, QString::fromUtf8(r.gender));
        xlsx.write(i + 2, 4, r.age);
        xlsx.write(i + 2, 5, QString::fromUtf8(r.dept));
        xlsx.write(i + 2, 6, QString::fromUtf8(r.diag));
        xlsx.write(i + 2, 7, QString::fromUtf8(r.admit));
        xlsx.write(i + 2, 8, QString::fromUtf8(r.discharge));
    }

    QDir().mkpath(QFileInfo(outPath).absolutePath());
    if (!xlsx.saveAs(outPath)) {
        qCritical() << "保存失败:" << outPath;
        return 1;
    }
    qInfo() << "已生成" << kCount << "条示例数据到:" << outPath;
    return 0;
}
