// ============================================================================
// 文件功能：图像识别页 OcrRecognitionWidget 声明。批量选图 → 调 OcrEngine 识别 →
//           人工修正字段 → 保存入库，并查看历史识别记录。
// ============================================================================
#pragma once

#include <QList>
#include <QMap>
#include <QStringList>
#include <QWidget>

class QLabel;
class QLineEdit;
class QListWidget;

class OcrRecognitionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OcrRecognitionWidget(QWidget *parent = nullptr);

private slots:
    void onSelectImages();
    void onRecognize();
    void onSave();
    void onHistory();
    void onImageSelected();

private:
    QString currentImagePath() const;
    void updateList();
    void loadFieldsForCurrentImage();
    void saveCurrentFieldEdits();

    QListWidget *imageList_;
    QList<QLineEdit *> fieldEdits_; // 5 个字段输入框
    QLabel *statusLabel_;

    QStringList images_;                                // 已选图片
    QString currentPath_;                               // 当前正在展示的图片
    QMap<QString, QMap<QString, QString>> fieldsByImage_; // 图片 -> 字段值
    QMap<QString, QString> statusByImage_;               // 图片 -> 成功/失败/未识别
};
