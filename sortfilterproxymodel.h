#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include "configurefile.h"
#include <QString>
#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    QModelIndexList pHighlightIndexs;
    QModelIndexList * pH;
    QModelIndexList markIndexs;
    QList<int> markRows;
    QString type;
    QModelIndex curHighlightRow;

    SortFilterProxyModel(QString typefile, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;

    QString filterMinimumDate() const { return minDate; }
    void setFilterMinimumDate(const QString &date);

    QString filterMaximumDate() const { return maxDate; }
    void setFilterMaximumDate(const QString &date);

    QString filterChoiceComponent() const { return cur_choice_component; }
    void setfilterChoiceComponent(const QString &component);

    QString filterChoiceOperation() const { return cur_choice_operation; }
    void setFilterChoiceOperation(const QString &operation);

    QString filterRegexp_event() const { return cur_regexp_event; }
    void setFilterRegexp_event(const QString &event);

    QModelIndex go_to_previous_row(int);
    QModelIndex go_to_next_row(int);

protected:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    //bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    bool dateInRange(const QString &date) const;

    QString minDate;
    QString maxDate;

    QString cur_choice_component;
    QString cur_choice_operation;
    QString cur_regexp_event;
};
#endif // SORTFILTERPROXYMODEL_H
