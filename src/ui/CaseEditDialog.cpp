// ============================================================================
// 文件功能：病例新增/编辑对话框实现。日期用文本输入（留空表示未填），
//           兼容 yyyy-MM-dd 与 yyyy/M/d 格式。
// ============================================================================
#include "CaseEditDialog.h"

#include <QComboBox>
#include <QDate>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>

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

CaseEditDialog::CaseEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("病例信息"));

    caseNoEdit_ = new QLineEdit(this);
    nameEdit_ = new QLineEdit(this);
    genderCombo_ = new QComboBox(this);
    genderCombo_->addItems({QString(), tr("男"), tr("女")});
    ageEdit_ = new QLineEdit(this);
    ageEdit_->setPlaceholderText(tr("留空表示未知"));
    deptEdit_ = new QLineEdit(this);
    diagEdit_ = new QLineEdit(this);
    admitEdit_ = new QLineEdit(this);
    admitEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));
    dischargeEdit_ = new QLineEdit(this);
    dischargeEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));

    auto *form = new QFormLayout;
    form->addRow(tr("病例号 *"), caseNoEdit_);
    form->addRow(tr("姓名"), nameEdit_);
    form->addRow(tr("性别"), genderCombo_);
    form->addRow(tr("年龄"), ageEdit_);
    form->addRow(tr("科室"), deptEdit_);
    form->addRow(tr("诊断"), diagEdit_);
    form->addRow(tr("入院日期"), admitEdit_);
    form->addRow(tr("出院日期"), dischargeEdit_);

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

void CaseEditDialog::setCase(const PatientCase &c)
{
    caseNoEdit_->setText(c.caseNo);
    nameEdit_->setText(c.patientName);
    genderCombo_->setCurrentText(c.gender);
    ageEdit_->setText(c.age >= 0 ? QString::number(c.age) : QString());
    deptEdit_->setText(c.department);
    diagEdit_->setText(c.diagnosis);
    admitEdit_->setText(c.admitDate.isValid() ? c.admitDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
    dischargeEdit_->setText(c.dischargeDate.isValid() ? c.dischargeDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
}

PatientCase CaseEditDialog::getCase() const
{
    PatientCase c;
    c.caseNo = caseNoEdit_->text().trimmed();
    c.patientName = nameEdit_->text().trimmed();
    c.gender = genderCombo_->currentText().trimmed();
    const QString ageStr = ageEdit_->text().trimmed();
    c.age = ageStr.isEmpty() ? -1 : ageStr.toInt();
    c.department = deptEdit_->text().trimmed();
    c.diagnosis = diagEdit_->text().trimmed();
    c.admitDate = parseFlexibleDate(admitEdit_->text());
    c.dischargeDate = parseFlexibleDate(dischargeEdit_->text());
    return c;
}

bool CaseEditDialog::validate()
{
    if (caseNoEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("病例号不能为空"));
        caseNoEdit_->setFocus();
        return false;
    }
    const QString ageStr = ageEdit_->text().trimmed();
    if (!ageStr.isEmpty()) {
        bool ok = false;
        const int age = ageStr.toInt(&ok);
        if (!ok || age < 0 || age > 150) {
            QMessageBox::warning(this, tr("提示"), tr("年龄必须是 0~150 的整数"));
            ageEdit_->setFocus();
            return false;
        }
    }
    for (QLineEdit *e : {admitEdit_, dischargeEdit_}) {
        if (!e->text().trimmed().isEmpty() && !parseFlexibleDate(e->text()).isValid()) {
            QMessageBox::warning(this, tr("提示"), tr("日期格式错误，请用 yyyy-MM-dd"));
            e->setFocus();
            return false;
        }
    }
    return true;
}
