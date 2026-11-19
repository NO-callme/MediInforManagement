// ============================================================================
// 文件功能：病例管理页实现。加载病例列表、筛选分页，并调用导入/编辑对话框
//           与 PatientCaseDao 完成增删改。
// ============================================================================
#include "CaseManagementWidget.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include "CaseEditDialog.h"
#include "ImportDialog.h"
#include "PatientCaseTableModel.h"
#include "dao/PatientCaseDao.h"

CaseManagementWidget::CaseManagementWidget(QWidget *parent)
    : QWidget(parent)
{
    // 顶部工具栏
    auto *importBtn = new QPushButton(tr("导入 Excel"), this);
    auto *addBtn = new QPushButton(tr("新增"), this);
    auto *editBtn = new QPushButton(tr("编辑"), this);
    auto *delBtn = new QPushButton(tr("删除"), this);
    auto *refreshBtn = new QPushButton(tr("刷新"), this);
    keywordEdit_ = new QLineEdit(this);
    keywordEdit_->setPlaceholderText(tr("搜索病例号/姓名/科室/诊断"));
    deptEdit_ = new QLineEdit(this);
    deptEdit_->setPlaceholderText(tr("科室"));
    deptEdit_->setFixedWidth(120);
    auto *searchBtn = new QPushButton(tr("查询"), this);
    auto *resetBtn = new QPushButton(tr("重置"), this);

    connect(importBtn, &QPushButton::clicked, this, &CaseManagementWidget::onImport);
    connect(addBtn, &QPushButton::clicked, this, &CaseManagementWidget::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &CaseManagementWidget::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &CaseManagementWidget::onRemove);
    connect(refreshBtn, &QPushButton::clicked, this, &CaseManagementWidget::refresh);
    connect(searchBtn, &QPushButton::clicked, this, &CaseManagementWidget::onSearch);
    connect(resetBtn, &QPushButton::clicked, this, &CaseManagementWidget::onReset);
    connect(keywordEdit_, &QLineEdit::returnPressed, this, &CaseManagementWidget::onSearch);

    auto *toolbar = new QHBoxLayout;
    toolbar->addWidget(importBtn);
    toolbar->addWidget(addBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);
    toolbar->addWidget(refreshBtn);
    toolbar->addSpacing(16);
    toolbar->addWidget(keywordEdit_, 1);
    toolbar->addWidget(deptEdit_);
    toolbar->addWidget(searchBtn);
    toolbar->addWidget(resetBtn);

    // 表格
    model_ = new PatientCaseTableModel(this);
    view_ = new QTableView(this);
    view_->setModel(model_);
    view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);
    view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view_->setAlternatingRowColors(true);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->setVisible(false);

    // 底部分页栏
    auto *prevBtn = new QPushButton(tr("上一页"), this);
    auto *nextBtn = new QPushButton(tr("下一页"), this);
    pageLabel_ = new QLabel(this);
    statusLabel_ = new QLabel(this);
    connect(prevBtn, &QPushButton::clicked, this, &CaseManagementWidget::onPrevPage);
    connect(nextBtn, &QPushButton::clicked, this, &CaseManagementWidget::onNextPage);

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

void CaseManagementWidget::refresh()
{
    PatientCaseDao dao;
    QString err;
    const PatientCaseFilter filter = currentFilter();
    totalCount_ = dao.count(filter, &err);

    const int pageCount = qMax(1, (totalCount_ + kPageSize - 1) / kPageSize);
    if (page_ >= pageCount)
        page_ = pageCount - 1;
    if (page_ < 0)
        page_ = 0;

    const QVector<PatientCase> list = dao.find(filter, kPageSize, page_ * kPageSize, &err);
    model_->setCases(list);
    pageLabel_->setText(tr("第 %1 / %2 页").arg(page_ + 1).arg(pageCount));
    statusLabel_->setText(err.isEmpty() ? tr("共 %1 条记录").arg(totalCount_)
                                        : tr("错误：%1").arg(err));
}

PatientCaseFilter CaseManagementWidget::currentFilter() const
{
    PatientCaseFilter f;
    f.keyword = keywordEdit_->text().trimmed();
    f.department = deptEdit_->text().trimmed();
    return f;
}

PatientCase CaseManagementWidget::currentSelected() const
{
    const QModelIndex idx = view_->currentIndex();
    if (!idx.isValid())
        return PatientCase();
    return model_->caseAt(idx.row());
}

void CaseManagementWidget::onImport()
{
    ImportDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted && dlg.imported()) {
        page_ = 0;
        refresh();
    }
}

void CaseManagementWidget::onAdd()
{
    CaseEditDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    PatientCase c = dlg.getCase();
    PatientCaseDao dao;
    QString err;
    if (dao.countByCaseNo(c.caseNo, &err) > 0) {
        QMessageBox::warning(this, tr("提示"), tr("病例号「%1」已存在").arg(c.caseNo));
        return;
    }
    if (!dao.insert(c, nullptr, &err)) {
        QMessageBox::critical(this, tr("新增失败"), err);
        return;
    }
    refresh();
}

void CaseManagementWidget::onEdit()
{
    const PatientCase c = currentSelected();
    if (c.id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    CaseEditDialog dlg(this);
    dlg.setCase(c);
    if (dlg.exec() != QDialog::Accepted)
        return;

    PatientCase updated = dlg.getCase();
    updated.id = c.id;
    PatientCaseDao dao;
    QString err;
    if (!dao.update(updated, &err)) {
        QMessageBox::critical(this, tr("保存失败"), err);
        return;
    }
    refresh();
}

void CaseManagementWidget::onRemove()
{
    const PatientCase c = currentSelected();
    if (c.id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一条记录"));
        return;
    }
    if (QMessageBox::question(this, tr("确认"), tr("确定删除病例「%1」吗？").arg(c.caseNo))
        != QMessageBox::Yes)
        return;

    PatientCaseDao dao;
    QString err;
    if (!dao.remove(c.id, &err)) {
        QMessageBox::critical(this, tr("删除失败"), err);
        return;
    }
    refresh();
}

void CaseManagementWidget::onSearch()
{
    page_ = 0;
    refresh();
}

void CaseManagementWidget::onReset()
{
    keywordEdit_->clear();
    deptEdit_->clear();
    page_ = 0;
    refresh();
}

void CaseManagementWidget::onPrevPage()
{
    if (page_ > 0) {
        --page_;
        refresh();
    }
}

void CaseManagementWidget::onNextPage()
{
    if ((page_ + 1) * kPageSize < totalCount_) {
        ++page_;
        refresh();
    }
}
