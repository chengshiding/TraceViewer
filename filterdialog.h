#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include "configurefile.h"
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

#include "sortfilterproxymodel.h"

class FilterDialog : public QDialog
{
    Q_OBJECT
public:
    FilterDialog(SortFilterProxyModel *proxy, QWidget *parent = 0);
public:

    QComboBox *componentCombox;
    QComboBox *operationCombox;

#if TIME_FILTER_IS_STRING
    QLineEdit *fromDateEdit;
    QLineEdit *toDateEdit;
#else
    QTimeEdit *fromDateEdit;
    QTimeEdit *toDateEdit;
#endif

private:
    QLabel *timeLabel;
    QLabel *fromLabel;
    QLabel *toLabel;
    QLabel *componentLabel;
    QLabel *operationLabel;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QHBoxLayout *hBoxLayout_0;
    QHBoxLayout *hBoxLayout_1;
    QHBoxLayout *hBoxLayout_2;
    QHBoxLayout *hBoxLayout_3;
    QVBoxLayout *mainLayout;
};

#endif // FILTERDIALOG_H
