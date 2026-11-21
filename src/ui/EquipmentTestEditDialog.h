// ============================================================================
// 文件功能：设备测试记录新增/编辑对话框 EquipmentTestEditDialog 声明。
// ============================================================================
#pragma once

#include <QDialog>

#include "model/EquipmentTest.h"

class QLineEdit;

class EquipmentTestEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EquipmentTestEditDialog(QWidget *parent = nullptr);

    void setTest(const EquipmentTest &t);
    EquipmentTest getTest() const;

private:
    QLineEdit *nameEdit_;
    QLineEdit *modelEdit_;
    QLineEdit *itemEdit_;
    QLineEdit *resultEdit_;
    QLineEdit *dateEdit_;
    QLineEdit *testerEdit_;
};
