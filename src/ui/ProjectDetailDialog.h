// ============================================================================
// 文件功能：项目详情对话框 ProjectDetailDialog 声明。含「里程碑 / 成员 / 设备测试」
//           三个标签页，支持各自的新增/编辑/删除，里程碑逾期红色高亮。
// ============================================================================
#pragma once

#include <QDialog>
#include <QString>

class QLineEdit;
class QTableWidget;

class ProjectDetailDialog : public QDialog
{
    Q_OBJECT

public:
    ProjectDetailDialog(quint32 projectId, const QString &projectName, QWidget *parent = nullptr);

private slots:
    void reloadMilestones();
    void reloadMembers();
    void reloadTests();
    void onMilestoneAdd();
    void onMilestoneEdit();
    void onMilestoneRemove();
    void onMemberAdd();
    void onMemberEdit();
    void onMemberRemove();
    void onTestAdd();
    void onTestEdit();
    void onTestRemove();

private:
    quint32 selectedId(QTableWidget *table) const;

    quint32 projectId_;
    QTableWidget *milestoneTable_;
    QTableWidget *memberTable_;
    QTableWidget *testTable_;
    QLineEdit *testSearchEdit_;
};
