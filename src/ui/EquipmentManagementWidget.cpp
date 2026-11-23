// ============================================================================
// 文件功能：设备台账管理页实现。加载列表、筛选分页、增删改。
// ============================================================================
#include "EquipmentManagementWidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include "EquipmentEditDialog.h"
#include "EquipmentTableModel.h"
#include "dao/EquipmentDao.h"

EquipmentManagementWidget::EquipmentManagementWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *addBtn = new QPushButton(tr("新增"), this);
    auto *editBtn = new QPushButton(tr("编辑"), this);
    auto *delBtn = new QPushButton(tr("删除"), this);
    auto *refreshBtn = new QPushButton(tr("刷新"), this);
    keywordEdit_ = new QLineEdit(this);
    keywordEdit_->setPlaceholderText(tr("搜索编号/名称/型号/科室"));
    statusCombo_ = new QComboBox(this);
    statusCombo_->addItems({tr("全部"), tr("在用"), tr("维修"), tr("报废")});
    auto *searchBtn = new QPushButton(tr("查询"), this);
    auto *resetBtn = new QPushButton(tr("重置"), this);

    connect(addBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::onAdd);
    connect(editBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::onEdit);
    connect(delBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::onRemove);
    connect(refreshBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::refresh);
    connect(searchBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::onSearch);
    connect(resetBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::onReset);
    connect(keywordEdit_, &QLineEdit::returnPressed, this, &EquipmentManagementWidget::onSearch);

    auto *toolbar = new QHBoxLayout;
    toolbar->addWidget(addBtn);
    toolbar->addWidget(editBtn);
    toolbar->addWidget(delBtn);
    toolbar->addWidget(refreshBtn);
    toolbar->addSpacing(16);
    toolbar->addWidget(keywordEdit_, 1);
    toolbar->addWidget(statusCombo_);
    toolbar->addWidget(searchBtn);
    toolbar->addWidget(resetBtn);

    model_ = new EquipmentTableModel(this);
    view_ = new QTableView(this);
    view_->setModel(model_);
    view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);
    view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view_->setAlternatingRowColors(true);
    view_->horizontalHeader()->setStretchLastSection(true);
    view_->verticalHeader()->setVisible(false);

    auto *prevBtn = new QPushButton(tr("上一页"), this);
    auto *nextBtn = new QPushButton(tr("下一页"), this);
    pageLabel_ = new QLabel(this);
    statusLabel_ = new QLabel(this);
    connect(prevBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::onPrevPage);
    connect(nextBtn, &QPushButton::clicked, this, &EquipmentManagementWidget::onNextPage);

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

void EquipmentManagementWidget::refresh()
{
    EquipmentDao dao;
    QString err;
    const EquipmentFilter filter = currentFilter();
    totalCount_ = dao.count(filter, &err);

    const int pageCount = qMax(1, (totalCount_ + kPageSize - 1) / kPageSize);
    if (page_ >= pageCount)
        page_ = pageCount - 1;
    if (page_ < 0)
        page_ = 0;

    const QVector<Equipment> list = dao.find(filter, kPageSize, page_ * kPageSize, &err);
    model_->setEquipments(list);
    pageLabel_->setText(tr("第 %1 / %2 页").arg(page_ + 1).arg(pageCount));
    statusLabel_->setText(err.isEmpty() ? tr("共 %1 台设备").arg(totalCount_)
                                        : tr("错误：%1").arg(err));
}

EquipmentFilter EquipmentManagementWidget::currentFilter() const
{
    EquipmentFilter f;
    f.keyword = keywordEdit_->text().trimmed();
    const QString status = statusCombo_->currentText();
    f.status = (status == tr("全部")) ? QString() : status;
    return f;
}

Equipment EquipmentManagementWidget::currentSelected() const
{
    const QModelIndex idx = view_->currentIndex();
    if (!idx.isValid())
        return Equipment();
    return model_->equipmentAt(idx.row());
}

void EquipmentManagementWidget::onAdd()
{
    EquipmentEditDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    Equipment e = dlg.getEquipment();
    EquipmentDao dao;
    QString err;
    if (dao.countByNo(e.equipmentNo, &err) > 0) {
        QMessageBox::warning(this, tr("提示"), tr("设备编号「%1」已存在").arg(e.equipmentNo));
        return;
    }
    if (!dao.insert(e, nullptr, &err)) {
        QMessageBox::critical(this, tr("新增失败"), err);
        return;
    }
    refresh();
}

void EquipmentManagementWidget::onEdit()
{
    const Equipment e = currentSelected();
    if (e.id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一台设备"));
        return;
    }
    EquipmentEditDialog dlg(this);
    dlg.setEquipment(e);
    if (dlg.exec() != QDialog::Accepted)
        return;
    Equipment updated = dlg.getEquipment();
    updated.id = e.id;
    EquipmentDao dao;
    QString err;
    if (!dao.update(updated, &err)) {
        QMessageBox::critical(this, tr("保存失败"), err);
        return;
    }
    refresh();
}

void EquipmentManagementWidget::onRemove()
{
    const Equipment e = currentSelected();
    if (e.id == 0) {
        QMessageBox::information(this, tr("提示"), tr("请先选择一台设备"));
        return;
    }
    if (QMessageBox::question(this, tr("确认"), tr("确定删除设备「%1」吗？").arg(e.name))
        != QMessageBox::Yes)
        return;
    EquipmentDao dao;
    QString err;
    if (!dao.remove(e.id, &err)) {
        QMessageBox::critical(this, tr("删除失败"), err);
        return;
    }
    refresh();
}

void EquipmentManagementWidget::onSearch()
{
    page_ = 0;
    refresh();
}

void EquipmentManagementWidget::onReset()
{
    keywordEdit_->clear();
    statusCombo_->setCurrentIndex(0);
    page_ = 0;
    refresh();
}

void EquipmentManagementWidget::onPrevPage()
{
    if (page_ > 0) {
        --page_;
        refresh();
    }
}

void EquipmentManagementWidget::onNextPage()
{
    if ((page_ + 1) * kPageSize < totalCount_) {
        ++page_;
        refresh();
    }
}
