// ============================================================================
// 文件功能：设备台账新增/编辑对话框实现。
// ============================================================================
#include "EquipmentEditDialog.h"

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

EquipmentEditDialog::EquipmentEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("设备台账"));

    noEdit_ = new QLineEdit(this);
    nameEdit_ = new QLineEdit(this);
    modelEdit_ = new QLineEdit(this);
    categoryEdit_ = new QLineEdit(this);
    statusCombo_ = new QComboBox(this);
    statusCombo_->addItems({QStringLiteral("在用"), QStringLiteral("维修"), QStringLiteral("报废")});
    purchaseEdit_ = new QLineEdit(this);
    purchaseEdit_->setPlaceholderText(QStringLiteral("yyyy-MM-dd"));
    deptEdit_ = new QLineEdit(this);
    remarkEdit_ = new QLineEdit(this);

    auto *form = new QFormLayout;
    form->addRow(tr("设备编号 *"), noEdit_);
    form->addRow(tr("设备名称 *"), nameEdit_);
    form->addRow(tr("型号"), modelEdit_);
    form->addRow(tr("分类"), categoryEdit_);
    form->addRow(tr("状态"), statusCombo_);
    form->addRow(tr("采购日期"), purchaseEdit_);
    form->addRow(tr("所属科室"), deptEdit_);
    form->addRow(tr("备注"), remarkEdit_);

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

void EquipmentEditDialog::setEquipment(const Equipment &e)
{
    noEdit_->setText(e.equipmentNo);
    nameEdit_->setText(e.name);
    modelEdit_->setText(e.model);
    categoryEdit_->setText(e.category);
    statusCombo_->setCurrentText(e.status);
    purchaseEdit_->setText(e.purchaseDate.isValid() ? e.purchaseDate.toString(QStringLiteral("yyyy-MM-dd")) : QString());
    deptEdit_->setText(e.department);
    remarkEdit_->setText(e.remark);
}

Equipment EquipmentEditDialog::getEquipment() const
{
    Equipment e;
    e.equipmentNo = noEdit_->text().trimmed();
    e.name = nameEdit_->text().trimmed();
    e.model = modelEdit_->text().trimmed();
    e.category = categoryEdit_->text().trimmed();
    e.status = statusCombo_->currentText();
    e.purchaseDate = parseFlexibleDate(purchaseEdit_->text());
    e.department = deptEdit_->text().trimmed();
    e.remark = remarkEdit_->text().trimmed();
    return e;
}

bool EquipmentEditDialog::validate()
{
    if (noEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("设备编号不能为空"));
        noEdit_->setFocus();
        return false;
    }
    if (nameEdit_->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("设备名称不能为空"));
        nameEdit_->setFocus();
        return false;
    }
    if (!purchaseEdit_->text().trimmed().isEmpty() && !parseFlexibleDate(purchaseEdit_->text()).isValid()) {
        QMessageBox::warning(this, tr("提示"), tr("采购日期格式错误，请用 yyyy-MM-dd"));
        purchaseEdit_->setFocus();
        return false;
    }
    return true;
}
