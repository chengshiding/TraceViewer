#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QStringList>
#include <QColor>
#include <QFont>
#include <QDateTime>
#include <QBrush>
#include "configurefile.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    QList<QStringList> listOfContent;
    qint64 pos;
    QStringList cur_row_modify;
    QDateTime cur_modify_time;
    QVector<bool> headDataColor;
    QModelIndexList markIndexs;  //corresponding to mouse_press_old(), current not used
    TableModel(int Columns, QStringList Names, QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRows(int position, int rows, const QModelIndex &index);

private:
    int ColumnCounts;
    QStringList ColumnNames;

};

#endif // TABLEMODEL_H
