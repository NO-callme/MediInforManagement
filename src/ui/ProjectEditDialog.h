// ============================================================================
// 文件功能：项目新增/编辑对话框 ProjectEditDialog 声明。
// ============================================================================
#pragma once

#include <QDialog>

#include "model/Project.h"

class QComboBox;
class QLineEdit;
class QSpinBox;

class ProjectEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectEditDialog(QWidget *parent = nullptr);

    void setProject(const Project &p);
    Project getProject() const;

private:
    bool validate();

    QLineEdit *noEdit_;
    QLineEdit *nameEdit_;
    QComboBox *statusCombo_;
    QLineEdit *ownerEdit_;
    QLineEdit *startEdit_;
    QLineEdit *endEdit_;
    QSpinBox *progressSpin_;
    QLineEdit *descEdit_;
};
