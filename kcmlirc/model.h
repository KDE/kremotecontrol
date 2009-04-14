
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
#include "iraction.h"
#include <QItemDelegate>
#include <QStandardItem>
//class QVariant
//class QString
#include <iraction.h>


Q_DECLARE_METATYPE(Prototype)
Q_DECLARE_METATYPE(IRAction*)

class DBusServiceItem : public QStandardItem
{

private:
    static QString trimAppname(const QString &appName);

public:
    DBusServiceItem(const QString &item);
    DBusServiceItem(const QString &item, QStringList &objects);

    virtual QVariant data(int  role) const;
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
    void setPrototypes( const QList <Prototype>  protoTypeList);

    int rowCount(const QModelIndex &parent=QModelIndex()) const {
        return theProtoTypeList.size();
    }

    int columnCount(const QModelIndex &parent=QModelIndex()) const {
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
    ArgumentsModelItem ( const QString & text );
    ArgumentsModelItem ( const QVariant &data);
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

