// ============================================================================
// 文件功能：项目详情对话框实现。三个标签页各自从 DAO 加载并维护子数据。
// ============================================================================
#include "ProjectDetailDialog.h"

#include <QColor>
#include <QDate>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

#include "EquipmentTestEditDialog.h"
#include "MemberEditDialog.h"
#include "MilestoneEditDialog.h"
#include "dao/EquipmentTestDao.h"
#include "dao/MilestoneDao.h"
#include "dao/ProjectMemberDao.h"

ProjectDetailDialog::ProjectDetailDialog(quint32 projectId, const QString &projectName, QWidget *parent)
    : QDialog(parent)
    , projectId_(projectId)
{
    setWindowTitle(tr("项目详情 - %1").arg(projectName));
    resize(760, 520);

    // ---- 里程碑页 ----
    milestoneTable_ = new QTableWidget(this);
    milestoneTable_->setColumnCount(4);
    milestoneTable_->setHorizontalHeaderLabels({tr("名称"), tr("计划日期"), tr("实际日期"), tr("状态")});
    milestoneTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    milestoneTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    milestoneTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    milestoneTable_->horizontalHeader()->setStretchLastSection(true);

    auto *msAdd = new QPushButton(tr("新增"), this);
    auto *msEdit = new QPushButton(tr("编辑"), this);
    auto *msDel = new QPushButton(tr("删除"), this);
    connect(msAdd, &QPushButton::clicked, this, &ProjectDetailDialog::onMilestoneAdd);
    connect(msEdit, &QPushButton::clicked, this, &ProjectDetailDialog::onMilestoneEdit);
    connect(msDel, &QPushButton::clicked, this, &ProjectDetailDialog::onMilestoneRemove);
    auto *msBtns = new QHBoxLayout;
    msBtns->addWidget(msAdd);
    msBtns->addWidget(msEdit);
    msBtns->addWidget(msDel);
    msBtns->addStretch();
    auto *msPage = new QWidget(this);
    auto *msLayout = new QVBoxLayout(msPage);
    msLayout->addWidget(milestoneTable_, 1);
    msLayout->addLayout(msBtns);

    // ---- 成员页 ----
    memberTable_ = new QTableWidget(this);
    memberTable_->setColumnCount(4);
    memberTable_->setHorizontalHeaderLabels({tr("姓名"), tr("角色/分工"), tr("电话"), tr("备注")});
    memberTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    memberTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    memberTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    memberTable_->horizontalHeader()->setStretchLastSection(true);

    auto *mbAdd = new QPushButton(tr("新增"), this);
    auto *mbEdit = new QPushButton(tr("编辑"), this);
    auto *mbDel = new QPushButton(tr("删除"), this);
    connect(mbAdd, &QPushButton::clicked, this, &ProjectDetailDialog::onMemberAdd);
    connect(mbEdit, &QPushButton::clicked, this, &ProjectDetailDialog::onMemberEdit);
    connect(mbDel, &QPushButton::clicked, this, &ProjectDetailDialog::onMemberRemove);
    auto *mbBtns = new QHBoxLayout;
    mbBtns->addWidget(mbAdd);
    mbBtns->addWidget(mbEdit);
    mbBtns->addWidget(mbDel);
    mbBtns->addStretch();
    auto *mbPage = new QWidget(this);
    auto *mbLayout = new QVBoxLayout(mbPage);
    mbLayout->addWidget(memberTable_, 1);
    mbLayout->addLayout(mbBtns);

    // ---- 设备测试页 ----
    testTable_ = new QTableWidget(this);
    testTable_->setColumnCount(6);
    testTable_->setHorizontalHeaderLabels(
        {tr("设备名称"), tr("型号"), tr("测试项"), tr("测试结果"), tr("测试日期"), tr("测试人")});
    testTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    testTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    testTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    testTable_->horizontalHeader()->setStretchLastSection(true);

    testSearchEdit_ = new QLineEdit(this);
    testSearchEdit_->setPlaceholderText(tr("搜索设备/测试项/测试人"));
    auto *testSearchBtn = new QPushButton(tr("筛选"), this);
    connect(testSearchBtn, &QPushButton::clicked, this, &ProjectDetailDialog::reloadTests);
    connect(testSearchEdit_, &QLineEdit::returnPressed, this, &ProjectDetailDialog::reloadTests);

    auto *tsAdd = new QPushButton(tr("新增"), this);
    auto *tsEdit = new QPushButton(tr("编辑"), this);
    auto *tsDel = new QPushButton(tr("删除"), this);
    connect(tsAdd, &QPushButton::clicked, this, &ProjectDetailDialog::onTestAdd);
    connect(tsEdit, &QPushButton::clicked, this, &ProjectDetailDialog::onTestEdit);
    connect(tsDel, &QPushButton::clicked, this, &ProjectDetailDialog::onTestRemove);
    auto *tsBtns = new QHBoxLayout;
    tsBtns->addWidget(tsAdd);
    tsBtns->addWidget(tsEdit);
    tsBtns->addWidget(tsDel);
    tsBtns->addStretch();
    tsBtns->addWidget(testSearchEdit_, 1);
    tsBtns->addWidget(testSearchBtn);

    auto *tsPage = new QWidget(this);
    auto *tsLayout = new QVBoxLayout(tsPage);
    tsLayout->addWidget(testTable_, 1);
    tsLayout->addLayout(tsBtns);

    // ---- 标签页 ----
    auto *tabs = new QTabWidget(this);
    tabs->addTab(msPage, tr("里程碑"));
    tabs->addTab(mbPage, tr("成员"));
    tabs->addTab(tsPage, tr("设备测试"));

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(tabs);

    reloadMilestones();
    reloadMembers();
    reloadTests();
}

quint32 ProjectDetailDialog::selectedId(QTableWidget *table) const
{
    const int row = table->currentRow();
    if (row < 0)
        return 0;
    QTableWidgetItem *it = table->item(row, 0);
    return it ? it->data(Qt::UserRole).toUInt() : 0;
}

void ProjectDetailDialog::reloadMilestones()
{
    MilestoneDao dao;
    QString err;
    const QVector<Milestone> list = dao.findByProject(projectId_, &err);
    milestoneTable_->setRowCount(list.size());
    const QDate today = QDate::currentDate();

    for (int i = 0; i < list.size(); ++i) {
        const Milestone &m = list.at(i);
        auto *nameItem = new QTableWidgetItem(m.name);
        nameItem->setData(Qt::UserRole, m.id);
        milestoneTable_->setItem(i, 0, nameItem);
        milestoneTable_->setItem(i, 1, new QTableWidgetItem(
            m.planDate.isValid() ? m.planDate.toString(QStringLiteral("yyyy-MM-dd")) : QString()));
        milestoneTable_->setItem(i, 2, new QTableWidgetItem(
            m.actualDate.isValid() ? m.actualDate.toString(QStringLiteral("yyyy-MM-dd")) : QString()));
        milestoneTable_->setItem(i, 3, new QTableWidgetItem(m.status));

        // 逾期预警：计划日期已过且未完成 -> 整行标红
        const bool overdue = m.planDate.isValid() && m.planDate < today && m.status != QStringLiteral("已完成");
        if (overdue) {
            for (int c = 0; c < milestoneTable_->columnCount(); ++c)
                milestoneTable_->item(i, c)->setBackground(QColor(255, 224, 224));
        }
    }
}

void ProjectDetailDialog::reloadMembers()
{
    ProjectMemberDao dao;
    QString err;
    const QVector<ProjectMember> list = dao.findByProject(projectId_, &err);
    memberTable_->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const ProjectMember &m = list.at(i);
        auto *nameItem = new QTableWidgetItem(m.name);
        nameItem->setData(Qt::UserRole, m.id);
        memberTable_->setItem(i, 0, nameItem);
        memberTable_->setItem(i, 1, new QTableWidgetItem(m.role));
        memberTable_->setItem(i, 2, new QTableWidgetItem(m.phone));
        memberTable_->setItem(i, 3, new QTableWidgetItem(m.remark));
    }
}

void ProjectDetailDialog::reloadTests()
{
    EquipmentTestDao dao;
    QString err;
    const QVector<EquipmentTest> list =
        dao.findByProject(projectId_, testSearchEdit_->text().trimmed(), &err);
    testTable_->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const EquipmentTest &t = list.at(i);
        auto *nameItem = new QTableWidgetItem(t.equipmentName);
        nameItem->setData(Qt::UserRole, t.id);
        testTable_->setItem(i, 0, nameItem);
        testTable_->setItem(i, 1, new QTableWidgetItem(t.model));
        testTable_->setItem(i, 2, new QTableWidgetItem(t.testItem));
        testTable_->setItem(i, 3, new QTableWidgetItem(t.testResult));
        testTable_->setItem(i, 4, new QTableWidgetItem(
            t.testDate.isValid() ? t.testDate.toString(QStringLiteral("yyyy-MM-dd")) : QString()));
        testTable_->setItem(i, 5, new QTableWidgetItem(t.tester));
    }
}

void ProjectDetailDialog::onMilestoneAdd()
{
    MilestoneEditDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    Milestone m = dlg.getMilestone();
    m.projectId = projectId_;
    MilestoneDao dao;
    QString err;
    if (!dao.insert(m, nullptr, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadMilestones();
}

void ProjectDetailDialog::onMilestoneEdit()
{
    const int row = milestoneTable_->currentRow();
    const quint32 id = selectedId(milestoneTable_);
    if (id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    Milestone m;
    m.id = id;
    m.projectId = projectId_;
    m.name = milestoneTable_->item(row, 0)->text();
    m.planDate = QDate::fromString(milestoneTable_->item(row, 1)->text(), QStringLiteral("yyyy-MM-dd"));
    m.actualDate = QDate::fromString(milestoneTable_->item(row, 2)->text(), QStringLiteral("yyyy-MM-dd"));
    m.status = milestoneTable_->item(row, 3)->text();

    MilestoneEditDialog dlg(this);
    dlg.setMilestone(m);
    if (dlg.exec() != QDialog::Accepted)
        return;
    Milestone updated = dlg.getMilestone();
    updated.id = m.id;
    updated.projectId = m.projectId;
    MilestoneDao dao;
    QString err;
    if (!dao.update(updated, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadMilestones();
}

void ProjectDetailDialog::onMilestoneRemove()
{
    const quint32 id = selectedId(milestoneTable_);
    if (id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    MilestoneDao dao;
    QString err;
    if (!dao.remove(id, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadMilestones();
}

void ProjectDetailDialog::onMemberAdd()
{
    MemberEditDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    ProjectMember m = dlg.getMember();
    m.projectId = projectId_;
    ProjectMemberDao dao;
    QString err;
    if (!dao.insert(m, nullptr, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadMembers();
}

void ProjectDetailDialog::onMemberEdit()
{
    const int row = memberTable_->currentRow();
    const quint32 id = selectedId(memberTable_);
    if (id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    ProjectMember m;
    m.id = id;
    m.projectId = projectId_;
    m.name = memberTable_->item(row, 0)->text();
    m.role = memberTable_->item(row, 1)->text();
    m.phone = memberTable_->item(row, 2)->text();
    m.remark = memberTable_->item(row, 3)->text();

    MemberEditDialog dlg(this);
    dlg.setMember(m);
    if (dlg.exec() != QDialog::Accepted)
        return;
    ProjectMember updated = dlg.getMember();
    updated.id = m.id;
    updated.projectId = m.projectId;
    ProjectMemberDao dao;
    QString err;
    if (!dao.update(updated, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadMembers();
}

void ProjectDetailDialog::onMemberRemove()
{
    const quint32 id = selectedId(memberTable_);
    if (id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    ProjectMemberDao dao;
    QString err;
    if (!dao.remove(id, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadMembers();
}

void ProjectDetailDialog::onTestAdd()
{
    EquipmentTestEditDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    EquipmentTest t = dlg.getTest();
    t.projectId = projectId_;
    EquipmentTestDao dao;
    QString err;
    if (!dao.insert(t, nullptr, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadTests();
}

void ProjectDetailDialog::onTestEdit()
{
    const int row = testTable_->currentRow();
    const quint32 id = selectedId(testTable_);
    if (id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    EquipmentTest t;
    t.id = id;
    t.projectId = projectId_;
    t.equipmentName = testTable_->item(row, 0)->text();
    t.model = testTable_->item(row, 1)->text();
    t.testItem = testTable_->item(row, 2)->text();
    t.testResult = testTable_->item(row, 3)->text();
    t.testDate = QDate::fromString(testTable_->item(row, 4)->text(), QStringLiteral("yyyy-MM-dd"));
    t.tester = testTable_->item(row, 5)->text();

    EquipmentTestEditDialog dlg(this);
    dlg.setTest(t);
    if (dlg.exec() != QDialog::Accepted)
        return;
    EquipmentTest updated = dlg.getTest();
    updated.id = t.id;
    updated.projectId = t.projectId;
    EquipmentTestDao dao;
    QString err;
    if (!dao.update(updated, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadTests();
}

void ProjectDetailDialog::onTestRemove()
{
    const quint32 id = selectedId(testTable_);
    if (id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    EquipmentTestDao dao;
    QString err;
    if (!dao.remove(id, &err)) {
        QMessageBox::critical(this, tr("失败"), err);
        return;
    }
    reloadTests();
}
