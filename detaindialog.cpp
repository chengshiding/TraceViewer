#include "detaindialog.h"
#include "configurefile.h"
#include <QDebug>

DetainDialog::DetainDialog(QModelIndex doubleClickedIndex, SortFilterProxyModel* proxy, QTableView* view, QStringList Names, QWidget *parent)
    : QDialog(parent),map_proxy(proxy),cur_view(view), ColumnNames(Names)
{   
    cur_index = doubleClickedIndex;
    cur_row = -1;

    if(map_proxy->type == "DirFile")
    {
        timeLabel = new QLabel(ColumnNames[0], this);
        timeValue = new QLabel(this);
        componentLabel = new QLabel(ColumnNames[1],this);
        componentValue = new QLabel(this);
        operationLabel = new QLabel(ColumnNames[2], this);
        operationValue = new QLabel(this);
        descriptionLabel = new QLabel(ColumnNames[3], this);
    }
    else if(map_proxy->type == "SingleFile")
    {
        timeLabel = new QLabel(ColumnNames[3], this);
        timeValue = new QLabel(this);
        componentLabel = new QLabel(ColumnNames[1],this);
        componentValue = new QLabel(this);
        operationLabel = new QLabel(ColumnNames[2], this);
        operationValue = new QLabel(this);
        descriptionLabel = new QLabel(ColumnNames[5], this);

    }
    else
    {
        qDebug() << "detain box error";
        exit(0);
    }
#if SHOW_EVENT_WITH_TEXTEDIT
    eventValue = new QTextEdit(this);
    eventValue->setReadOnly(true);
    //eventValue->setBackgroundRole(Qt::gray);
    //eventValue->setStyleSheet("QTextEdit { background: lightGray }");

#else
    eventValue = new QLabel(this);
    eventValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
    eventValue->setFrameShadow(QFrame::Sunken);
    eventValue->setFrameShape(QFrame::Panel);
    eventValue->adjustSize();
    //eventValue->setGeometry(QRect(328, 240, 329, 27*4));  //四倍行距
    eventValue->setWordWrap(true);
    eventValue->setAlignment(Qt::AlignTop);
#endif
    closeButton = new QPushButton(tr("Close"), this);
    previousButton = new QPushButton(tr("Previous"), this);
    nextButton = new QPushButton(tr("Next"), this);

    //cur_row = map_proxy->mapToSource(cur_index).row();
    cur_row = cur_index.row();
    output();

    connect(previousButton, SIGNAL(clicked()), this, SLOT(toPrevious()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(this, SIGNAL(cur_IndexChanged()), this, SLOT(updateButtons()));


    gridLayout = new QGridLayout();
    gridLayout->addWidget(timeLabel, 0, 0, 1, 1);
    gridLayout->addWidget(timeValue, 0, 1, 1, 1);
    gridLayout->addWidget(componentLabel, 1, 0, 1, 1);
    gridLayout->addWidget(componentValue, 1, 1, 1, 1);
    gridLayout->addWidget(operationLabel, 2, 0, 1, 1);
    gridLayout->addWidget(operationValue, 2, 1, 1, 1);
    gridLayout->addWidget(descriptionLabel, 3, 0, 1, 1);
    gridLayout->addWidget(eventValue, 4, 0, 2, -1);

    hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(closeButton);
    //hBoxLayout->addSpacing(50);
    hBoxLayout->addStretch(10);
    hBoxLayout->addWidget(previousButton);
    hBoxLayout->addWidget(nextButton);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gridLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(hBoxLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Detain Box"));
    resize(80, 60);
}
void DetainDialog::toPrevious()
{
    if(cur_row > 0)
    {
        cur_row--;
        emit cur_IndexChanged();
        output();
    }
}
void DetainDialog::toNext()
{
    if(cur_row < (map_proxy->rowCount() - 1))
    {
        cur_row++;
        emit cur_IndexChanged();
        output();
    }
}

void DetainDialog::output()
{
    QString time;
    QString component;
    QString operation;
    QString event;

    QModelIndex TimeIndex;

    if(cur_row == -1)
    {
        qDebug() <<"output(): cur_row = -1";
        return;
    }
    if(map_proxy->type == "DirFile")
    {
        TimeIndex = map_proxy->index(cur_row, 0, QModelIndex());
        QVariant varTime = map_proxy->data(TimeIndex, Qt::DisplayRole);
        time = varTime.toString();

        QModelIndex ComponentIndex = map_proxy->index(cur_row, 1, QModelIndex());
        QVariant varComponent = map_proxy->data(ComponentIndex, Qt::DisplayRole);
        component = varComponent.toString();

        QModelIndex OperationIndex = map_proxy->index(cur_row, 2, QModelIndex());
        QVariant varOperation = map_proxy->data(OperationIndex, Qt::DisplayRole);
        operation = varOperation.toString();

        QModelIndex EventIndex = map_proxy->index(cur_row, 3, QModelIndex());
        QVariant varEvent = map_proxy->data(EventIndex, Qt::DisplayRole);
        event = varEvent.toString();
    }
    else if(map_proxy->type == "SingleFile")
    {
        TimeIndex = map_proxy->index(cur_row, 3, QModelIndex());
        QVariant varTime = map_proxy->data(TimeIndex, Qt::DisplayRole);
        time = varTime.toString();

        QModelIndex ComponentIndex = map_proxy->index(cur_row, 1, QModelIndex());
        QVariant varComponent = map_proxy->data(ComponentIndex, Qt::DisplayRole);
        component = varComponent.toString();

        QModelIndex OperationIndex = map_proxy->index(cur_row, 2, QModelIndex());
        QVariant varOperation = map_proxy->data(OperationIndex, Qt::DisplayRole);
        operation = varOperation.toString();

        QModelIndex EventIndex = map_proxy->index(cur_row, 5, QModelIndex());
        QVariant varEvent = map_proxy->data(EventIndex, Qt::DisplayRole);
        event = varEvent.toString();
    }
    timeValue->setText(time);
    componentValue->setText(component);
    operationValue->setText(operation);
    eventValue->setText(event);
    cur_view->setCurrentIndex(TimeIndex);
}

void DetainDialog::updateButtons()
{
    previousButton->setEnabled(cur_row > 0);
    nextButton->setEnabled(cur_row < map_proxy->rowCount() - 1);
    //qDebug() << "updateButtons(): cur_row = " << cur_row;
}
