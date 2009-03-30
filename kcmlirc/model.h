
/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/

/*
 *
 *  Created on: 01.02.2009
 *      @author Frank Scheffold
 */

#ifndef MODEL_H_
#define MODEL_H_


#include "prototype.h"

#include <QStringListModel>
#include <QItemDelegate>
#include <QStandardItem>

//class QVariant
//class QString


Q_DECLARE_METATYPE(Prototype)


class DBusProfileModel : public  QStringListModel
{

public:

    DBusProfileModel (QObject * parent );

    virtual QVariant data( const QModelIndex & index, int  role) const;
    static QString trimAppname(const QString &appName);
    static bool decendingLessThan(const QString &s1, const QString &s2);
    static bool ascendingLessThan(const QString &s1, const QString &s2);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);


};


class DBusFunctionModel : public  QAbstractListModel
{
private:
    QList <Prototype>  theProtoTypeList;

public:

    DBusFunctionModel (QObject * parent );


    virtual QVariant data( const QModelIndex & index, int  role) const;
    virtual bool setData(const QModelIndex &index,  const QVariant &value, int role);
    bool insertRows(int position, int rows, const QModelIndex &parent);
    bool removeRows(int position, int rows, const QModelIndex &parent);

    int rowCount(const QModelIndex &parent) const {
        return theProtoTypeList.size();
    }

    int columnCount(const QModelIndex &parent) const {
        return 3;
    }

    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags =0) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void sort(int column, Qt::SortOrder order);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

class ArgumentsModelItem: public QStandardItem
{
public:
    virtual QVariant data ( int role = Qt::UserRole + 1 ) const;

private:

};


class ArgumentDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ArgumentDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

};


#endif /* MODEL_H_ */
