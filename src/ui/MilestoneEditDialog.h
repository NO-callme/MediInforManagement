// ============================================================================
// 文件功能：里程碑新增/编辑对话框 MilestoneEditDialog 声明。
// ============================================================================
#pragma once

#include <QDialog>

#include "model/Milestone.h"

class QComboBox;
class QLineEdit;

class MilestoneEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MilestoneEditDialog(QWidget *parent = nullptr);

    void setMilestone(const Milestone &m);
    Milestone getMilestone() const;

private:
    QLineEdit *nameEdit_;
    QLineEdit *planEdit_;
    QLineEdit *actualEdit_;
    QComboBox *statusCombo_;
};
