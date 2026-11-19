// ============================================================================
// 文件功能：病例管理页 CaseManagementWidget 声明。提供病例列表、关键字/科室
//           筛选、分页，以及导入/新增/编辑/删除操作入口。
// ============================================================================
#pragma once

#include <QWidget>

#include "model/PatientCase.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class PatientCaseTableModel;

class CaseManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CaseManagementWidget(QWidget *parent = nullptr);

    void refresh(); // 按当前筛选条件与页码重新加载

private slots:
    void onImport();
    void onAdd();
    void onEdit();
    void onRemove();
    void onSearch();
    void onReset();
    void onPrevPage();
    void onNextPage();

private:
    PatientCaseFilter currentFilter() const;
    PatientCase currentSelected() const;

    QLineEdit *keywordEdit_;
    QLineEdit *deptEdit_;
    QTableView *view_;
    PatientCaseTableModel *model_;
    QLabel *statusLabel_;
    QLabel *pageLabel_;

    int page_ = 0;                 // 当前页（0 起）
    int totalCount_ = 0;
    static constexpr int kPageSize = 20;
};
