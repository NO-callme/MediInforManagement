// ============================================================================
// 文件功能：项目成员新增/编辑对话框 MemberEditDialog 声明。
// ============================================================================
#pragma once

#include <QDialog>

#include "model/ProjectMember.h"

class QLineEdit;

class MemberEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemberEditDialog(QWidget *parent = nullptr);

    void setMember(const ProjectMember &m);
    ProjectMember getMember() const;

private:
    QLineEdit *nameEdit_;
    QLineEdit *roleEdit_;
    QLineEdit *phoneEdit_;
    QLineEdit *remarkEdit_;
};
