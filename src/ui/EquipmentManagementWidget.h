// ============================================================================
// 文件功能：设备台账管理页 EquipmentManagementWidget 声明。列表 + 筛选分页 +
//           新增/编辑/删除操作。
// ============================================================================
#pragma once

#include <QWidget>

#include "model/Equipment.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class EquipmentTableModel;

class EquipmentManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EquipmentManagementWidget(QWidget *parent = nullptr);

    void refresh();

private slots:
    void onAdd();
    void onEdit();
    void onRemove();
    void onSearch();
    void onReset();
    void onPrevPage();
    void onNextPage();

private:
    EquipmentFilter currentFilter() const;
    Equipment currentSelected() const;

    QLineEdit *keywordEdit_;
    QComboBox *statusCombo_;
    QTableView *view_;
    EquipmentTableModel *model_;
    QLabel *statusLabel_;
    QLabel *pageLabel_;

    int page_ = 0;
    int totalCount_ = 0;
    static constexpr int kPageSize = 20;
};
