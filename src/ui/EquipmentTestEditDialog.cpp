// ============================================================================
// 文件功能：设备测试记录新增/编辑对话框实现。
// ============================================================================
#include "EquipmentTestEditDialog.h"

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

EquipmentTestEditDialog::EquipmentTestEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("设备测试"));

    nameEdit_ = new QLineEdit(this);
    modelEdit_ = new QLineEdit(this);
    itemEdit_ = new QLineEdit(this);
    resultEdit_ = new QLineEdit(this);
    dateEdit_ = new QLineEdit(this);
    dateEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));
    testerEdit_ = new QLineEdit(this);

    auto *form = new QFormLayout;
    form->addRow(tr("设备名称 *"), nameEdit_);
    form->addRow(tr("型号"), modelEdit_);
    form->addRow(tr("测试项"), itemEdit_);
    form->addRow(tr("测试结果"), resultEdit_);
    form->addRow(tr("测试日期"), dateEdit_);
    form->addRow(tr("测试人"), testerEdit_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (nameEdit_->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("设备名称不能为空"));
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

void EquipmentTestEditDialog::setTest(const EquipmentTest &t)
{
    nameEdit_->setText(t.equipmentName);
    modelEdit_->setText(t.model);
    itemEdit_->setText(t.testItem);
    resultEdit_->setText(t.testResult);
    dateEdit_->setText(t.testDate.isValid() ? t.testDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
    testerEdit_->setText(t.tester);
}

EquipmentTest EquipmentTestEditDialog::getTest() const
{
    EquipmentTest t;
    t.equipmentName = nameEdit_->text().trimmed();
    t.model = modelEdit_->text().trimmed();
    t.testItem = itemEdit_->text().trimmed();
    t.testResult = resultEdit_->text().trimmed();
    t.testDate = parseFlexibleDate(dateEdit_->text());
    t.tester = testerEdit_->text().trimmed();
    return t;
}
