// ============================================================================
// 文件功能：Excel 导入向导实现。解析预览 + 校验 + 事务导入，失败行红色高亮。
// ============================================================================
#include "ImportDialog.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QColor>

#include "service/CaseImportService.h"

ImportDialog::ImportDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("导入 Excel 病例数据"));
    resize(720, 520);

    fileEdit_ = new QLineEdit(this);
    fileEdit_->setPlaceholderText(tr("选择 .xlsx 文件"));
    auto *browseBtn = new QPushButton(tr("浏览…"), this);
    connect(browseBtn, &QPushButton::clicked, this, &ImportDialog::onBrowse);

    auto *fileRow = new QHBoxLayout;
    fileRow->addWidget(fileEdit_, 1);
    fileRow->addWidget(browseBtn);

    preview_ = new QTableWidget(this);
    preview_->setColumnCount(6);
    preview_->setHorizontalHeaderLabels(
        {tr("行号"), tr("病例号"), tr("姓名"), tr("科室"), tr("诊断"), tr("校验结果")});
    preview_->horizontalHeader()->setStretchLastSection(true);
    preview_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    preview_->setSelectionBehavior(QAbstractItemView::SelectRows);

    statusLabel_ = new QLabel(tr("请选择 Excel 文件"), this);

    auto *validateBtn = new QPushButton(tr("仅校验"), this);
    importBtn_ = new QPushButton(tr("校验并导入"), this);
    importBtn_->setEnabled(false);
    auto *cancelBtn = new QPushButton(tr("关闭"), this);
    connect(validateBtn, &QPushButton::clicked, this, &ImportDialog::onValidate);
    connect(importBtn_, &QPushButton::clicked, this, &ImportDialog::onImport);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    auto *btnRow = new QHBoxLayout;
    btnRow->addWidget(statusLabel_, 1);
    btnRow->addWidget(validateBtn);
    btnRow->addWidget(importBtn_);
    btnRow->addWidget(cancelBtn);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(fileRow);
    layout->addWidget(preview_, 1);
    layout->addLayout(btnRow);
}

bool ImportDialog::imported() const
{
    return imported_;
}

void ImportDialog::onBrowse()
{
    const QString file = QFileDialog::getOpenFileName(this, tr("选择 Excel 文件"),
                                                      QString(), tr("Excel 文件 (*.xlsx)"));
    if (file.isEmpty())
        return;
    fileEdit_->setText(file);
    currentFile_ = file;
    importBtn_->setEnabled(true);
}

void ImportDialog::onValidate()
{
    currentFile_ = fileEdit_->text().trimmed();
    if (currentFile_.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择 Excel 文件"));
        return;
    }
    parsed_ = CaseImportService::validateFile(currentFile_);
    if (!parsed_.ok) {
        statusLabel_->setText(parsed_.error);
        return;
    }
    fillPreview(parsed_);
    importBtn_->setEnabled(true);
}

void ImportDialog::onImport()
{
    currentFile_ = fileEdit_->text().trimmed();
    if (currentFile_.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择 Excel 文件"));
        return;
    }

    QString err;
    const CaseImportService::ImportSummary s = CaseImportService::importFile(currentFile_, tr("admin"), &err);
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("导入失败"), err);
        return;
    }

    statusLabel_->setText(tr("导入完成：共 %1 行，成功 %2 行，失败 %3 行")
                              .arg(s.total).arg(s.success).arg(s.failed));
    imported_ = (s.success > 0);
    QMessageBox::information(this, tr("导入结果"),
                             tr("共 %1 行，成功 %2 行，失败 %3 行。")
                                 .arg(s.total).arg(s.success).arg(s.failed));
}

void ImportDialog::fillPreview(const ExcelParseResult &result)
{
    preview_->setRowCount(0);
    preview_->setRowCount(result.rows.size());

    int valid = 0;
    int invalid = 0;
    for (int i = 0; i < result.rows.size(); ++i) {
        const ParsedCaseRow &r = result.rows.at(i);
        preview_->setItem(i, 0, new QTableWidgetItem(QString::number(r.rowNumber)));
        preview_->setItem(i, 1, new QTableWidgetItem(r.data.caseNo));
        preview_->setItem(i, 2, new QTableWidgetItem(r.data.patientName));
        preview_->setItem(i, 3, new QTableWidgetItem(r.data.department));
        preview_->setItem(i, 4, new QTableWidgetItem(r.data.diagnosis));
        preview_->setItem(i, 5, new QTableWidgetItem(r.error.isEmpty() ? tr("通过") : r.error));

        if (r.error.isEmpty()) {
            ++valid;
        } else {
            ++invalid;
            for (int c = 0; c < preview_->columnCount(); ++c)
                preview_->item(i, c)->setBackground(QColor(255, 224, 224));
        }
    }

    statusLabel_->setText(tr("校验完成：通过 %1 行，失败 %2 行").arg(valid).arg(invalid));
    preview_->resizeColumnsToContents();
}
