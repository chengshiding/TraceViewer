#ifndef DETAINDIALOG_H
#define DETAINDIALOG_H

#include "sortfilterproxymodel.h"
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QTimeEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QGridLayout>
#include <QDataWidgetMapper>
#include <QTableView>
#include "tablemodel.h"

class DetainDialog : public QDialog
{
    Q_OBJECT
public:
    DetainDialog(QModelIndex doubleClickedIndex, SortFilterProxyModel* proxy, QTableView* view, QStringList Names, QWidget *parent = 0);
    void output();
public slots:
    void updateButtons();
    void toPrevious();
    void toNext();

signals:
    void cur_IndexChanged();

private:
    QLabel *timeLabel;
    QLabel *timeValue;
    QLabel *componentLabel;
    QLabel *componentValue;
    QLabel *operationLabel;
    QLabel *operationValue;
    QLabel *descriptionLabel;

#if SHOW_EVENT_WITH_TEXTEDIT
    QTextEdit  *eventValue;
#else
    QLabel *eventValue;
#endif

    QPushButton *previousButton;
    QPushButton *nextButton;
    QPushButton *closeButton;

    QHBoxLayout *hBoxLayout;
    QVBoxLayout *mainLayout;
    QGridLayout *gridLayout;

    //QDataWidgetMapper *mapper;
    SortFilterProxyModel* map_proxy;
    QTableView* cur_view;
    QModelIndex cur_index;
    int cur_row;
    int ColumnCounts;
    QStringList ColumnNames;
};
#endif // DETAINDIALOG_H
