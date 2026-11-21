// ============================================================================
// 文件功能：项目管理页实现。加载项目列表、筛选分页，删除项目时级联清理
//           里程碑/成员/设备测试，双击打开详情对话框。
// ============================================================================
#include "ProjectManagementWidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include "ProjectDetailDialog.h"
#include "ProjectEditDialog.h"
#include "ProjectTableModel.h"
#include "dao/EquipmentTestDao.h"
#include "dao/MilestoneDao.h"
#include "dao/ProjectDao.h"
#include "dao/ProjectMemberDao.h"

ProjectManagementWidget::ProjectManagementWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *addBtn = new QPushButton(tr("新增"), this);
    auto *editBtn = new QPushButton(tr("编辑"), this);
    auto *delBtn = new QPushButton(tr("删除"), this);
    auto *detailBtn = new QPushButton(tr("详情"), this);
    auto *refreshBtn = new QPushButton(tr("刷新"), this);
    keywordEdit_ = new QLineEdit(this);
    keywordEdit_->setPlaceholderText(tr("搜索编号/名称/负责人"));
    statusCombo_ = new QComboBox(this);
    statusCombo_->addItems({tr("全部"), tr("进行中"), tr("已完成"), tr("已延期")});
    auto *searchBtn = new QPushButton(tr("查询"), this);
    auto *resetBtn = new QPushButton(tr("重置"), this);

    connect(addBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onRemove);
    connect(detailBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onDetail);
    connect(refreshBtn, &QPushButton::clicked, this, &ProjectManagementWidget::refresh);
    connect(searchBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onSearch);
    connect(resetBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onReset);
    connect(keywordEdit_, &QLineEdit::returnPressed, this, &ProjectManagementWidget::onSearch);

    auto *toolbar = new QHBoxLayout;
    toolbar->addWidget(addBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);
    toolbar->addWidget(detailBtn);
    toolbar->addWidget(refreshBtn);
    toolbar->addSpacing(16);
    toolbar->addWidget(keywordEdit_, 1);
    toolbar->addWidget(statusCombo_);
    toolbar->addWidget(searchBtn);
    toolbar->addWidget(resetBtn);

    model_ = new ProjectTableModel(this);
    view_ = new QTableView(this);
    view_->setModel(model_);
    view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);
    view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view_->setAlternatingRowColors(true);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->setVisible(false);
    connect(view_, &QTableView::doubleClicked, this, &ProjectManagementWidget::onDoubleClicked);

    auto *prevBtn = new QPushButton(tr("上一页"), this);
    auto *nextBtn = new QPushButton(tr("下一页"), this);
    pageLabel_ = new QLabel(this);
    statusLabel_ = new QLabel(this);
    connect(prevBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onPrevPage);
    connect(nextBtn, &QPushButton::clicked, this, &ProjectManagementWidget::onNextPage);

    auto *bottom = new QHBoxLayout;
    bottom->addWidget(statusLabel_, 1);
    bottom->addWidget(prevBtn);
    bottom->addWidget(pageLabel_);
    bottom->addWidget(nextBtn);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(toolbar);
    layout->addWidget(view_, 1);
    layout->addLayout(bottom);

    refresh();
}

void ProjectManagementWidget::refresh()
{
    ProjectDao dao;
    QString err;
    const ProjectFilter filter = currentFilter();
    totalCount_ = dao.count(filter, &err);

    const int pageCount = qMax(1, (totalCount_ + kPageSize - 1) / kPageSize);
    if (page_ >= pageCount)
        page_ = pageCount - 1;
    if (page_ < 0)
        page_ = 0;

    const QVector<Project> list = dao.find(filter, kPageSize, page_ * kPageSize, &err);
    model_->setProjects(list);
    pageLabel_->setText(tr("第 %1 / %2 页").arg(page_ + 1).arg(pageCount));
    statusLabel_->setText(err.isEmpty() ? tr("共 %1 个项目").arg(totalCount_)
                                        : tr("错误：%1").arg(err));
}

ProjectFilter ProjectManagementWidget::currentFilter() const
{
    ProjectFilter f;
    f.keyword = keywordEdit_->text().trimmed();
    const QString status = statusCombo_->currentText();
    f.status = (status == tr("全部")) ? QString() : status;
    return f;
}

Project ProjectManagementWidget::currentSelected() const
{
    const QModelIndex idx = view_->currentIndex();
    if (!idx.isValid())
        return Project();
    return model_->projectAt(idx.row());
}

void ProjectManagementWidget::onAdd()
{
    ProjectEditDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    Project p = dlg.getProject();
    ProjectDao dao;
    QString err;
    if (dao.countByProjectNo(p.projectNo, &err) > 0) {
        QMessageBox::warning(this, tr("提示"), tr("项目编号「%1」已存在").arg(p.projectNo));
        return;
    }
    if (!dao.insert(p, nullptr, &err)) {
        QMessageBox::critical(this, tr("新增失败"), err);
        return;
    }
    refresh();
}

void ProjectManagementWidget::onEdit()
{
    const Project p = currentSelected();
    if (p.id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一个项目"));
        return;
    }
    ProjectEditDialog dlg(this);
    dlg.setProject(p);
    if (dlg.exec() != QDialog::Accepted)
        return;
    Project updated = dlg.getProject();
    updated.id = p.id;
    ProjectDao dao;
    QString err;
    if (!dao.update(updated, &err)) {
        QMessageBox::critical(this, tr("保存失败"), err);
        return;
    }
    refresh();
}

void ProjectManagementWidget::onRemove()
{
    const Project p = currentSelected();
    if (p.id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一个项目"));
        return;
    }
    if (QMessageBox::question(this, tr("确认"),
                              tr("删除项目「%1」会同时删除其里程碑、成员和设备测试数据，确定？").arg(p.name))
        != QMessageBox::Yes)
        return;

    // 级联清理子表
    MilestoneDao().removeByProject(p.id);
    ProjectMemberDao().removeByProject(p.id);
    EquipmentTestDao().removeByProject(p.id);

    ProjectDao dao;
    QString err;
    if (!dao.remove(p.id, &err)) {
        QMessageBox::critical(this, tr("删除失败"), err);
        return;
    }
    refresh();
}

void ProjectManagementWidget::onDetail()
{
    const Project p = currentSelected();
    if (p.id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一个项目"));
        return;
    }
    ProjectDetailDialog dlg(p.id, p.name, this);
    dlg.exec();
}

void ProjectManagementWidget::onSearch()
{
    page_ = 0;
    refresh();
}

void ProjectManagementWidget::onReset()
{
    keywordEdit_->clear();
    statusCombo_->setCurrentIndex(0);
    page_ = 0;
    refresh();
}

void ProjectManagementWidget::onPrevPage()
{
    if (page_ > 0) {
        --page_;
        refresh();
    }
}

void ProjectManagementWidget::onNextPage()
{
    if ((page_ + 1) * kPageSize < totalCount_) {
        ++page_;
        refresh();
    }
}

void ProjectManagementWidget::onDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    onDetail();
}
