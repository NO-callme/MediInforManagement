// ============================================================================
// 文件功能：Excel 导入向导 ImportDialog 声明。选择文件 → 解析预览（失败行标红）
//           → 校验并导入，导入结果反馈。
// ============================================================================
#pragma once

#include <QDialog>

#include "excel/ExcelCaseParser.h"

class QLineEdit;
class QTableWidget;
class QLabel;
class QPushButton;

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = nullptr);

    // 导入成功后返回 true（供调用方刷新列表）
    bool imported() const;

private slots:
    void onBrowse();
    void onValidate();
    void onImport();

private:
    void fillPreview(const ExcelParseResult &result);

    QLineEdit *fileEdit_;
    QTableWidget *preview_;
    QLabel *statusLabel_;
    QPushButton *importBtn_;

    QString currentFile_;
    ExcelParseResult parsed_;
    bool imported_ = false;
};
