#include "filterdialog.h"
#include <QDebug>

FilterDialog::FilterDialog(SortFilterProxyModel *proxy, QWidget *parent)
    : QDialog(parent)
{
    timeLabel =  new QLabel;
    fromLabel = new QLabel;
    toLabel = new QLabel;

#if TIME_FILTER_IS_STRING
    fromDateEdit = new QLineEdit;
    toDateEdit = new QLineEdit;
#else
    fromDateEdit = new QTimeEdit;
    toDateEdit = new QTimeEdit;
#endif

    componentLabel = new QLabel;
    componentCombox = new QComboBox;

    operationLabel = new QLabel;
    operationCombox = new QComboBox;

    okButton = new QPushButton(tr("OK"));
    cancelButton = new QPushButton(tr("Cancel"));

    hBoxLayout_0 = new QHBoxLayout;
    hBoxLayout_1 = new QHBoxLayout;
    hBoxLayout_2 = new QHBoxLayout;
    hBoxLayout_3 = new QHBoxLayout;

    mainLayout = new QVBoxLayout;

    QString dateString = proxy->filterMinimumDate();

    if(dateString.isEmpty())
    {
        qDebug() << "filterdialog(): MinimumDate = """;
#if TIME_FILTER_IS_STRING
        fromDateEdit->setText("");
#else
        fromDateEdit->setTime(QTime());
        qDebug()<< "FilterDialog() : fromDateEdit->setTime(QTime()) " <<QTime().toString();
#endif

    }
#if TIME_FILTER_IS_STRING
    else
        fromDateEdit->setText(dateString);
#endif

    dateString = proxy->filterMaximumDate();
    if(dateString.isEmpty())
    {
#if TIME_FILTER_IS_STRING
        toDateEdit->setText("");
#else
        toDateEdit->setTime(QTime());
        qDebug()<< "FilterDialog() : toDateEdit->setTime(QTime()) " <<QTime().toString();
#endif

    }
#if TIME_FILTER_IS_STRING
    else
        toDateEdit->setText(dateString);
#endif

    timeLabel->setText(tr("TIME:"));
    fromLabel->setText(tr("From:"));
    toLabel->setText(tr("To:"));
    hBoxLayout_0->addWidget(timeLabel);
    hBoxLayout_0->addWidget(fromLabel);
    hBoxLayout_0->addWidget(fromDateEdit);
    hBoxLayout_0->addWidget(toLabel);
    hBoxLayout_0->addWidget(toDateEdit);
    QStringList components;
    QStringList operations;
    if(proxy->type == "DirFile")
    {
        components << "" << "OCX" << "EXE" << "GET" << "USR";
        operations << "" << "CALL" << "CMD" << "RSLT" << "xEVT" << "oEVT";
    }
    else if(proxy->type == "SingleFile")
    {
        components << "" << "RegisterThread" << "FlowEngine" << "SetData" << "CheckEvent" << "DisplayPage" << "DataOper" << "ExecuteScript" << "RTLWnd" << "脚本引擎";
        operations << "" << "User_Flow" << "Monitor_Flow" << "Manager_Flow" << "RunTimeLib";
    }
    else
    {
        qDebug() << "FilterDialog(): proxy->type ERROR";
    }
    componentLabel->setText(tr("COMPONENTS"));
    componentCombox->addItems(components);
    componentCombox->setCurrentText(proxy->filterChoiceComponent());

    operationLabel->setText(tr("OPERATIONS"));
    operationCombox->addItems(operations);
    operationCombox->setCurrentText(proxy->filterChoiceOperation());

    hBoxLayout_1->addWidget(componentLabel);
    hBoxLayout_1->addWidget(componentCombox);

    hBoxLayout_2->addWidget(operationLabel);
    hBoxLayout_2->addWidget(operationCombox);

    hBoxLayout_3->addWidget(cancelButton);
    hBoxLayout_3->addWidget(okButton);

    mainLayout->addLayout(hBoxLayout_0);
    mainLayout->addLayout(hBoxLayout_1);
    mainLayout->addLayout(hBoxLayout_2);
    mainLayout->addLayout(hBoxLayout_3);

    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle(tr("Choose Filter Dialog"));
    //resize(250, 140);
}
