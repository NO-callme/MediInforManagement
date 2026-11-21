// ============================================================================
// 文件功能：项目成员新增/编辑对话框实现。
// ============================================================================
#include "MemberEditDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

MemberEditDialog::MemberEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("项目成员"));

    nameEdit_ = new QLineEdit(this);
    roleEdit_ = new QLineEdit(this);
    phoneEdit_ = new QLineEdit(this);
    remarkEdit_ = new QLineEdit(this);

    auto *form = new QFormLayout;
    form->addRow(tr("姓名 *"), nameEdit_);
    form->addRow(tr("角色/分工"), roleEdit_);
    form->addRow(tr("电话"), phoneEdit_);
    form->addRow(tr("备注"), remarkEdit_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (nameEdit_->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("姓名不能为空"));
            nameEdit_->setFocus();
            return;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

void MemberEditDialog::setMember(const ProjectMember &m)
{
    nameEdit_->setText(m.name);
    roleEdit_->setText(m.role);
    phoneEdit_->setText(m.phone);
    remarkEdit_->setText(m.remark);
}

ProjectMember MemberEditDialog::getMember() const
{
    ProjectMember m;
    m.name = nameEdit_->text().trimmed();
    m.role = roleEdit_->text().trimmed();
    m.phone = phoneEdit_->text().trimmed();
    m.remark = remarkEdit_->text().trimmed();
    return m;
}
