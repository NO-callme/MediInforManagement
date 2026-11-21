// ============================================================================
// 文件功能：里程碑新增/编辑对话框实现。
// ============================================================================
#include "MilestoneEditDialog.h"

#include <QComboBox>
#include <QDate>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

namespace {
QDate parseFlexibleDate(const QString &s)
{
    const QString t = s.trimmed();
    if (t.isEmpty())
        return QDate();
    const char *fmts[] = {"yyyy-MM-dd", "yyyy/M/d", "yyyy/MM/dd"};
    for (const char *f : fmts) {
        const QDate d = QDate::fromString(t, QLatin1String(f));
        if (d.isValid())
            return d;
    }
    return QDate();
}
} // namespace

MilestoneEditDialog::MilestoneEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("里程碑"));

    nameEdit_ = new QLineEdit(this);
    planEdit_ = new QLineEdit(this);
    planEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));
    actualEdit_ = new QLineEdit(this);
    actualEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));
    statusCombo_ = new QComboBox(this);
    statusCombo_->addItems({QStringLiteral("未开始"), QStringLiteral("进行中"), QStringLiteral("已完成")});

    auto *form = new QFormLayout;
    form->addRow(tr("名称 *"), nameEdit_);
    form->addRow(tr("计划日期"), planEdit_);
    form->addRow(tr("实际日期"), actualEdit_);
    form->addRow(tr("状态"), statusCombo_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (nameEdit_->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("名称不能为空"));
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

void MilestoneEditDialog::setMilestone(const Milestone &m)
{
    nameEdit_->setText(m.name);
    planEdit_->setText(m.planDate.isValid() ? m.planDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
    actualEdit_->setText(m.actualDate.isValid() ? m.actualDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
    statusCombo_->setCurrentText(m.status);
}

Milestone MilestoneEditDialog::getMilestone() const
{
    Milestone m;
    m.name = nameEdit_->text().trimmed();
    m.planDate = parseFlexibleDate(planEdit_->text());
    m.actualDate = parseFlexibleDate(actualEdit_->text());
    m.status = statusCombo_->currentText();
    return m;
}
