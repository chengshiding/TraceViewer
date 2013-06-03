#include <QDebug>
#include "tablemodel.h"

TableModel::TableModel(int Columns, QStringList Names, QObject *parent)
    : QAbstractTableModel(parent)
{
    ColumnCounts = Columns;
    ColumnNames = Names;
    //pHighlightIndexs = pH;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return listOfContent.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCounts;
}

bool TableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    Q_UNUSED(rows);

    //insert a row
    beginInsertRows(QModelIndex(), position, position);
    listOfContent.append(cur_row_modify);

    //insert many row
    /*
    beginInsertRows(QModelIndex(), position, position + rows);
    for (int row = 0; row < rows; ++row)
    {
        listOfContent.append(cur_row_modify);
    }
    */
    endInsertRows();
    return true;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= listOfContent.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        QStringList rowContent = listOfContent.at(index.row());

        switch(index.column())
        {
            case 0:
                return (rowContent[0].isEmpty()) ? QVariant() : rowContent[0];
            case 1:
                return (rowContent[1].isEmpty()) ? QVariant() : rowContent[1];
            case 2:
                return (rowContent[2].isEmpty()) ? QVariant() : rowContent[2];
            case 3:
                return (rowContent[3].isEmpty()) ? QVariant() : rowContent[3];
            case 4:
                return (rowContent[4].isEmpty()) ? QVariant() : rowContent[4];
            case 5:
                return (rowContent[5].isEmpty()) ? QVariant() : rowContent[5];
            default:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{  
    if(role == Qt::ForegroundRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
                case 0:
                if(headDataColor[0])
                    return QVariant(QColor(Qt::red));
                else
                    return QAbstractTableModel::headerData(section,orientation,role);

                case 1:
                if(headDataColor[1])
                    return QVariant(QColor(Qt::red));
                else
                    return QAbstractTableModel::headerData(section,orientation,role);

                case 2:
                if(headDataColor[2])
                    return QVariant(QColor(Qt::red));
                else
                    return QAbstractTableModel::headerData(section,orientation,role);

                case 3:
                if(headDataColor[3])
                    return QVariant(QColor(Qt::red));
                else
                    return QAbstractTableModel::headerData(section,orientation,role);
                case 4:
                if(headDataColor[4])
                    return QVariant(QColor(Qt::red));
                else
                    return QAbstractTableModel::headerData(section,orientation,role);

                case 5:
                if(headDataColor[5])
                    return QVariant(QColor(Qt::red));
                else
                    return QAbstractTableModel::headerData(section,orientation,role);

                default:
                    return QAbstractTableModel::headerData(section,orientation,role);
            }
        }
    }
    else if(role == Qt::BackgroundRole)
    {
        //if(orientation == Qt::Vertical)
        if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                //qDebug() << "blue";
                return 	QVariant(QColor(Qt::blue));
            case 1:
                return 	QBrush(QColor(Qt::blue));
            default:
                return QAbstractTableModel::headerData(section,orientation,role);
        }
    }
    else if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
                case 0:
                    return ColumnNames[0];

                case 1:
                    return ColumnNames[1];

                case 2:
                    return ColumnNames[2];

                case 3:
                    return ColumnNames[3];

                case 4:
                    return ColumnNames[4];

                case 5:
                    return ColumnNames[5];

                default:
                    return QAbstractTableModel::headerData(section,orientation,role);
            }
        }
    }
    else if(role == Qt::FontRole)
    {
        if (orientation == Qt::Horizontal)
        {
            return QVariant(QFont("Times", 11, QFont::Bold));
        }
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}
