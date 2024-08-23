#ifndef ONB7_H
#define ONB7_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
namespace Ui {
class onb7;
}

class onb7 : public QDialog
{
    Q_OBJECT

public:
    explicit onb7(QWidget *parent = nullptr);
    ~onb7();
private slots:
    void selectAll();
    void on_btnExit_clicked();
    void add();
    void remove();
    void clean();
    void edit();
    void on_tw_itemSelectionChanged();
    void search();
    void generateReportRequest();
private:
    Ui::onb7 *ui;
    void populate1ComboBox();
    void populate2ComboBox();
    void populate3ComboBox();

    QString m_lastSearchQuery;
    int m_lastFoundIndex = -1;
};

#endif // ONB7_H
