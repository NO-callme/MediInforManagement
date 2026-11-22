// ============================================================================
// 文件功能：图像识别页实现。默认使用模拟引擎，识别结果可手动修正后入库。
// ============================================================================
#include "OcrRecognitionWidget.h"

#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "dao/OcrRecordDao.h"
#include "ocr/OcrEngineFactory.h"

namespace {
// 固定识别的字段（与模拟引擎输出一致）
const char *const kFieldNames[] = {"病例号", "姓名", "性别", "科室", "诊断"};
constexpr int kFieldCount = sizeof(kFieldNames) / sizeof(kFieldNames[0]);
} // namespace

OcrRecognitionWidget::OcrRecognitionWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *selectBtn = new QPushButton(tr("选择图片"), this);
    auto *recognizeBtn = new QPushButton(tr("识别"), this);
    auto *saveBtn = new QPushButton(tr("保存入库"), this);
    auto *historyBtn = new QPushButton(tr("历史记录"), this);
    connect(selectBtn, &QPushButton::clicked, this, &OcrRecognitionWidget::onSelectImages);
    connect(recognizeBtn, &QPushButton::clicked, this, &OcrRecognitionWidget::onRecognize);
    connect(saveBtn, &QPushButton::clicked, this, &OcrRecognitionWidget::onSave);
    connect(historyBtn, &QPushButton::clicked, this, &OcrRecognitionWidget::onHistory);

    auto *toolbar = new QHBoxLayout;
    toolbar->addWidget(selectBtn);
    toolbar->addWidget(recognizeBtn);
    toolbar->addWidget(saveBtn);
    toolbar->addWidget(historyBtn);
    toolbar->addStretch();

    imageList_ = new QListWidget(this);
    imageList_->setFixedWidth(320);
    connect(imageList_, &QListWidget::currentRowChanged, this, &OcrRecognitionWidget::onImageSelected);

    auto *form = new QFormLayout;
    for (int i = 0; i < kFieldCount; ++i) {
        auto *edit = new QLineEdit(this);
        fieldEdits_.append(edit);
        form->addRow(QString::fromUtf8(kFieldNames[i]), edit);
    }
    statusLabel_ = new QLabel(this);
    form->addRow(tr("状态"), statusLabel_);

    auto *right = new QVBoxLayout;
    right->addLayout(form);
    right->addStretch();

    auto *body = new QHBoxLayout;
    body->addWidget(imageList_);
    body->addLayout(right, 1);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(toolbar);
    layout->addLayout(body, 1);
}

QString OcrRecognitionWidget::currentImagePath() const
{
    const int row = imageList_->currentRow();
    if (row < 0 || row >= images_.size())
        return QString();
    return images_.at(row);
}

void OcrRecognitionWidget::updateList()
{
    imageList_->clear();
    for (const QString &path : images_) {
        QString label = QFileInfo(path).fileName();
        const QString status = statusByImage_.value(path);
        if (status == QStringLiteral("成功"))
            label += QStringLiteral("  [成功]");
        else if (status == QStringLiteral("失败"))
            label += QStringLiteral("  [失败]");
        imageList_->addItem(label);
    }
}

void OcrRecognitionWidget::loadFieldsForCurrentImage()
{
    currentPath_ = currentImagePath();
    const QString path = currentPath_;
    const QMap<QString, QString> fields = path.isEmpty() ? QMap<QString, QString>() : fieldsByImage_.value(path);
    for (int i = 0; i < kFieldCount; ++i)
        fieldEdits_[i]->setText(fields.value(QString::fromUtf8(kFieldNames[i])));
    statusLabel_->setText(path.isEmpty() ? QString() : statusByImage_.value(path));
}

void OcrRecognitionWidget::saveCurrentFieldEdits()
{
    const QString path = currentPath_;
    if (path.isEmpty())
        return;
    QMap<QString, QString> fields;
    for (int i = 0; i < kFieldCount; ++i)
        fields.insert(QString::fromUtf8(kFieldNames[i]), fieldEdits_[i]->text().trimmed());
    fieldsByImage_[path] = fields;
}

void OcrRecognitionWidget::onImageSelected()
{
    saveCurrentFieldEdits();  // 保存上一个图片的修改
    loadFieldsForCurrentImage();
}

void OcrRecognitionWidget::onSelectImages()
{
    const QStringList files = QFileDialog::getOpenFileNames(
        this, tr("选择图片"), QString(),
        tr("图片文件 (*.png *.jpg *.jpeg *.bmp)"));
    if (files.isEmpty())
        return;
    for (const QString &f : files) {
        if (!images_.contains(f)) {
            images_.append(f);
            statusByImage_[f] = QStringLiteral("未识别");
        }
    }
    updateList();
}

void OcrRecognitionWidget::onRecognize()
{
    if (images_.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请先选择图片"));
        return;
    }

    auto engine = OcrEngineFactory::create();
    for (const QString &path : images_) {
        const OcrResult r = engine->recognize(path);
        if (r.ok) {
            fieldsByImage_[path] = r.fields;
            statusByImage_[path] = QStringLiteral("成功");
        } else {
            fieldsByImage_[path].clear();
            statusByImage_[path] = QStringLiteral("失败");
        }
    }
    updateList();
    loadFieldsForCurrentImage();
}

void OcrRecognitionWidget::onSave()
{
    saveCurrentFieldEdits();

    OcrRecordDao dao;
    QString err;
    int saved = 0;
    for (const QString &path : images_) {
        const QString status = statusByImage_.value(path);
        if (status.isEmpty() || status == QStringLiteral("未识别"))
            continue;

        OcrRecord r;
        r.imagePath = path;
        r.status = status;
        r.operatorName = QStringLiteral("admin");
        if (status == QStringLiteral("成功")) {
            QJsonObject obj;
            const QMap<QString, QString> fields = fieldsByImage_.value(path);
            for (auto it = fields.constBegin(); it != fields.constEnd(); ++it)
                obj.insert(it.key(), it.value());
            r.recognizedJson = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
        }

        if (!dao.insert(r, nullptr, &err)) {
            QMessageBox::critical(this, tr("保存失败"), err);
            return;
        }
        ++saved;
    }

    QMessageBox::information(this, tr("完成"), tr("已保存 %1 条识别记录").arg(saved));
}

void OcrRecognitionWidget::onHistory()
{
    OcrRecordDao dao;
    QString err;
    const QVector<OcrRecord> list = dao.findAll(200, 0, &err);

    auto *dlg = new QDialog(this);
    dlg->setWindowTitle(tr("识别历史记录"));
    dlg->resize(680, 420);

    auto *table = new QTableWidget(dlg);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({tr("图片路径"), tr("状态"), tr("时间"), tr("识别结果")});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i) {
        const OcrRecord &r = list.at(i);
        table->setItem(i, 0, new QTableWidgetItem(r.imagePath));
        table->setItem(i, 1, new QTableWidgetItem(r.status));
        table->setItem(i, 2, new QTableWidgetItem(
            r.createTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))));
        table->setItem(i, 3, new QTableWidgetItem(r.recognizedJson));
    }

    auto *layout = new QVBoxLayout(dlg);
    layout->addWidget(table);
    dlg->exec();
    delete dlg;
}
