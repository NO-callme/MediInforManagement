// ============================================================================
// 文件功能：病例新增/编辑对话框 CaseEditDialog 声明，用于手工录入或修改病例。
// ============================================================================
#pragma once

#include <QDialog>

#include "model/PatientCase.h"

class QLineEdit;
class QComboBox;

class CaseEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CaseEditDialog(QWidget *parent = nullptr);

    void setCase(const PatientCase &c); // 编辑模式：填充现有值
    PatientCase getCase() const;        // 读取表单值（调用前先校验）

private:
    bool validate();

    QLineEdit *caseNoEdit_;
    QLineEdit *nameEdit_;
    QComboBox *genderCombo_;
    QLineEdit *ageEdit_;
    QLineEdit *deptEdit_;
    QLineEdit *diagEdit_;
    QLineEdit *admitEdit_;
    QLineEdit *dischargeEdit_;
};
