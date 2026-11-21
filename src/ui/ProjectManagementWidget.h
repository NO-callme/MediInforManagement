// ============================================================================
// 文件功能：项目管理页 ProjectManagementWidget 声明。项目列表 + 筛选分页 +
//           新增/编辑/删除/详情操作，双击行打开详情。
// ============================================================================
#pragma once

#include <QWidget>

#include "model/Project.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class ProjectTableModel;

class ProjectManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectManagementWidget(QWidget *parent = nullptr);

    void refresh();

private slots:
    void onAdd();
    void onEdit();
    void onRemove();
    void onDetail();
    void onSearch();
    void onReset();
    void onPrevPage();
    void onNextPage();
    void onDoubleClicked(const QModelIndex &index);

private:
    ProjectFilter currentFilter() const;
    Project currentSelected() const;

    QLineEdit *keywordEdit_;
    QComboBox *statusCombo_;
    QTableView *view_;
    ProjectTableModel *model_;
    QLabel *statusLabel_;
    QLabel *pageLabel_;

    int page_ = 0;
    int totalCount_ = 0;
    static constexpr int kPageSize = 20;
};
