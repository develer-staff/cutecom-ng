/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief History-style model class header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef HISTORYMODEL_H
#define HISTORYMODEL_

#include <QAbstractListModel>
#include <QStringList>

class HistoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit HistoryModel(QObject *parent = 0);
    explicit HistoryModel(const QStringList &strings, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    QStringList stringList() const;
    void setStringList(const QStringList &strings);

    Qt::DropActions supportedDropActions() const;

private:
    Q_DISABLE_COPY(HistoryModel)
    QStringList lst;
};

#endif // HISTORYMODEL_
