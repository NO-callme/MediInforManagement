// ============================================================================
// 文件功能：项目新增/编辑对话框实现。日期用文本输入（yyyy-MM-dd，留空=未定）。
// ============================================================================
#include "ProjectEditDialog.h"

#include <QComboBox>
#include <QDate>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {
QDate parseFlexibleDate(const QString &s)
{
    const QString t = s.trimmed();
    if (t.isEmpty())
        return QDate();
    const char *fmts[] = {"yyyy-MM-dd", "yyyy/M/d", "yyyy/M/dd", "yyyy/MM/dd"};
    for (const char *f : fmts) {
        const QDate d = QDate::fromString(t, QLatin1String(f));
        if (d.isValid())
            return d;
    }
    return QDate();
}
} // namespace

ProjectEditDialog::ProjectEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("项目信息"));

    noEdit_ = new QLineEdit(this);
    nameEdit_ = new QLineEdit(this);
    statusCombo_ = new QComboBox(this);
    statusCombo_->addItems({QStringLiteral("进行中"), QStringLiteral("已完成"), QStringLiteral("已延期")});
    ownerEdit_ = new QLineEdit(this);
    startEdit_ = new QLineEdit(this);
    startEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));
    endEdit_ = new QLineEdit(this);
    endEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));
    progressSpin_ = new QSpinBox(this);
    progressSpin_->setRange(0, 100);
    progressSpin_->setSuffix(QStringLiteral("%"));
    descEdit_ = new QLineEdit(this);

    auto *form = new QFormLayout;
    form->addRow(tr("项目编号 *"), noEdit_);
    form->addRow(tr("项目名称 *"), nameEdit_);
    form->addRow(tr("状态"), statusCombo_);
    form->addRow(tr("负责人"), ownerEdit_);
    form->addRow(tr("开始日期"), startEdit_);
    form->addRow(tr("结束日期"), endEdit_);
    form->addRow(tr("进度"), progressSpin_);
    form->addRow(tr("描述"), descEdit_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (validate())
            accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

void ProjectEditDialog::setProject(const Project &p)
{
    noEdit_->setText(p.projectNo);
    nameEdit_->setText(p.name);
    statusCombo_->setCurrentText(p.status);
    ownerEdit_->setText(p.owner);
    startEdit_->setText(p.startDate.isValid() ? p.startDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
    endEdit_->setText(p.endDate.isValid() ? p.endDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
    progressSpin_->setValue(p.progress);
    descEdit_->setText(p.description);
}

Project ProjectEditDialog::getProject() const
{
    Project p;
    p.projectNo = noEdit_->text().trimmed();
    p.name = nameEdit_->text().trimmed();
    p.status = statusCombo_->currentText();
    p.owner = ownerEdit_->text().trimmed();
    p.startDate = parseFlexibleDate(startEdit_->text());
    p.endDate = parseFlexibleDate(endEdit_->text());
    p.progress = progressSpin_->value();
    p.description = descEdit_->text().trimmed();
    return p;
}

bool ProjectEditDialog::validate()
{
    if (noEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("项目编号不能为空"));
        noEdit_->setFocus();
        return false;
    }
    if (nameEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("项目名称不能为空"));
        nameEdit_->setFocus();
        return false;
    }
    for (QLineEdit *e : {startEdit_, endEdit_}) {
        if (!e->text().trimmed().isEmpty() && !parseFlexibleDate(e->text()).isValid()) {
            QMessageBox::warning(this, tr("提示"), tr("日期格式错误，请用 yyyy-MM-dd"));
            e->setFocus();
            return false;
        }
    }
    return true;
}
