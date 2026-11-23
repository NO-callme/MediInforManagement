// ============================================================================
// 文件功能：设备台账新增/编辑对话框 EquipmentEditDialog 声明。
// ============================================================================
#pragma once

#include <QDialog>

#include "model/Equipment.h"

class QComboBox;
class QLineEdit;

class EquipmentEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EquipmentEditDialog(QWidget *parent = nullptr);

    void setEquipment(const Equipment &e);
    Equipment getEquipment() const;

private:
    bool validate();

    QLineEdit *noEdit_;
    QLineEdit *nameEdit_;
    QLineEdit *modelEdit_;
    QLineEdit *categoryEdit_;
    QComboBox *statusCombo_;
    QLineEdit *purchaseEdit_;
    QLineEdit *deptEdit_;
    QLineEdit *remarkEdit_;
};
