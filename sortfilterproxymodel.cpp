#include <QtWidgets>
#include <QDebug>
#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel(QString typefile, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    pH = &pHighlightIndexs;
    type = typefile;
}

void SortFilterProxyModel::setFilterMinimumDate(const QString &date)
{
    minDate = date;
    invalidateFilter();
}

void SortFilterProxyModel::setFilterMaximumDate(const QString &date)
{
    maxDate = date;
    invalidateFilter();
}

void SortFilterProxyModel::setfilterChoiceComponent(const QString &component)
{
    cur_choice_component = component;
    invalidateFilter();
}

void SortFilterProxyModel::setFilterChoiceOperation(const QString &operation)
{
    cur_choice_operation = operation;
    invalidateFilter();
}

void SortFilterProxyModel::setFilterRegexp_event(const QString &event)
{
    cur_regexp_event = event;
    invalidateFilter();
}
/*
bool SortFilterProxyModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows);
    //beginInsertRows(QModelIndex(), position, position);

    for (int row = 0; row < rows; ++row)
    {
        listOfContent.append(cur_row_modify);
        qDebug() << "add row " << row;
    }

    endInsertRows();
    return true;
}
*/
QVariant SortFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role ==Qt::BackgroundRole)
    {
        //if (orientation == Qt::Vertical);
            //if(markRows.contains(section+1))
            //{
                //qDebug() << "contains(section)=" << section;
                //return QVariant(QColor(Qt::blue));
            //}

    }
    return QSortFilterProxyModel::headerData(section,orientation,role);
}
QVariant SortFilterProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
/*
    if(role == Qt::FontRole)
    {
        if(markIndexs.contains(index))
            return QVariant(QFont("Times", 10, QFont::Bold));
    }
*/
    if(role == Qt::BackgroundRole)
    {
        if(markIndexs.contains(index))
            return QVariant(QColor(Qt::blue));

#if NOT_HIDE_ROWS_WHEN_FILTER
        if(pH->contains(index))
            return QVariant(QColor(Qt::yellow));
#endif

    }
    return QSortFilterProxyModel::data(index, role);
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    if(type == "DirFile")
    {
        QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
        QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
        QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
        QModelIndex index3 = sourceModel()->index(sourceRow, 3, sourceParent);

#if NOT_HIDE_ROWS_WHEN_FILTER
        if(!filterRegExp().isEmpty())
        {
            if(sourceModel()->data(index3).toString().contains(filterRegExp()))
            {
                if(!pHighlightIndexs.contains(index3))
                {
                    pH->append(mapFromSource(index3));
                }
            }
        }
        //return true;
        return (dateInRange(sourceModel()->data(index0).toString())
                && sourceModel()->data(index1).toString().contains(cur_choice_component)
                && sourceModel()->data(index2).toString().contains(cur_choice_operation));

#else
        return (dateInRange(sourceModel()->data(index0).toString())
                && sourceModel()->data(index1).toString().contains(cur_choice_component)
                && sourceModel()->data(index2).toString().contains(cur_choice_operation)
                && sourceModel()->data(index3).toString().contains(filterRegExp()));
#endif
    }
    else if(type == "SingleFile")
    {
        QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
        QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
        QModelIndex index3 = sourceModel()->index(sourceRow, 3, sourceParent);
        QModelIndex index5 = sourceModel()->index(sourceRow, 5, sourceParent);

#if NOT_HIDE_ROWS_WHEN_FILTER
        if(!filterRegExp().isEmpty())
        {
            if(sourceModel()->data(index5).toString().contains(filterRegExp()))
            {
                if(!pHighlightIndexs.contains(index5))
                {
                    pH->append(mapFromSource(index5));
                }
            }
        }
        //return true;
        return (sourceModel()->data(index1).toString().contains(cur_choice_component)
                && sourceModel()->data(index2).toString().contains(cur_choice_operation)
                && dateInRange(sourceModel()->data(index3).toString()));

#else
        return (dateInRange(sourceModel()->data(index0).toString())
                && sourceModel()->data(index1).toString().contains(cur_choice_component)
                && sourceModel()->data(index2).toString().contains(cur_choice_operation)
                && sourceModel()->data(index3).toString().contains(filterRegExp()));
#endif
    }
    else
        return true;
}
QModelIndex SortFilterProxyModel::go_to_previous_row(int row)
{
    int goLineIndex;
    if(row == -1)
    {
        goLineIndex = pHighlightIndexs.indexOf(curHighlightRow)-1;
    }
    else
    {
        for(goLineIndex=pHighlightIndexs.size()-1; goLineIndex>=0; --goLineIndex)
        {
            if(row > pHighlightIndexs.at(goLineIndex).row())
                break;
        }
    }
    if(goLineIndex >= 0)
    {
        curHighlightRow = pHighlightIndexs.at(goLineIndex);
        return curHighlightRow;
    }
    else
        return QModelIndex();
}

QModelIndex SortFilterProxyModel::go_to_next_row(int row)
{
    int goLineIndex;
    if(row == -1)
    {
        goLineIndex = pHighlightIndexs.indexOf(curHighlightRow)+1;
    }
    else
    {
        for(goLineIndex=0; goLineIndex<pHighlightIndexs.size(); ++goLineIndex)
        {
            if(row < pHighlightIndexs.at(goLineIndex).row())
                break;
        }
    }
    if(goLineIndex < pHighlightIndexs.size())
    {
        curHighlightRow = pHighlightIndexs.at(goLineIndex);
        return curHighlightRow;
    }
    else
        return QModelIndex();
}

/*
bool SortFilterProxyModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    if (leftData.type() == QVariant::DateTime)
    {
        return leftData.toDateTime() < rightData.toDateTime();
    } else
    {
        QRegExp *emailPattern = new QRegExp("([\\w\\.]*@[\\w\\.]*)");

        QString leftString = leftData.toString();
        if(left.column() == 1 && emailPattern->indexIn(leftString) != -1)
            leftString = emailPattern->cap(1);

        QString rightString = rightData.toString();
        if(right.column() == 1 && emailPattern->indexIn(rightString) != -1)
            rightString = emailPattern->cap(1);

        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
}
*/
bool SortFilterProxyModel::dateInRange(const QString &date) const
{
    if(minDate.isEmpty() && maxDate.isEmpty())
        return true;
    else if(minDate.isEmpty() && !maxDate.isEmpty())
    {
        return (date <= maxDate);
    }
    else if(!minDate.isEmpty() && maxDate.isEmpty())
    {
        return (date >= minDate);
    }
    else
        return (date >= minDate && date <= maxDate);
}
