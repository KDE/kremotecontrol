
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

#ifndef MODEL_H
#define MODEL_H


#include "prototype.h"
#include "iraction.h"
#include <QItemDelegate>
#include <QStandardItem>
#include <solid/control/remotecontrol.h>
#include <QStringListModel>

using namespace Solid::Control;
Q_DECLARE_METATYPE(Prototype)
Q_DECLARE_METATYPE(IRAction*)
Q_DECLARE_METATYPE(ProfileAction*)
Q_DECLARE_METATYPE(RemoteControlButton*)


class DBusServiceItem : public QStandardItem
{

private:
    static QString trimAppname(const QString &appName);

public:
    DBusServiceItem(const QString &item);
    DBusServiceItem(const QString &item, const QStringList &objects);

    virtual QVariant data(int  role) const;
};

class DBusFunctionModel: public QStandardItemModel
{
    public:
	DBusFunctionModel(QObject *parent);
	
	void appendRow ( const Prototype &item );
	
	Prototype getPrototype( int index ) const;
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


class ProfileModel: public QStandardItemModel
{
    public:
      ProfileModel(QObject *parent=0);
	ProfileModel(const Profile *profile, QObject *parent=0);
	
	ProfileAction* getProfileAction( int index ) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	void appendRow(ProfileAction *action);
	Qt::ItemFlags flags(const QModelIndex& index) const;
};



class RemoteButtonModel: public QStandardItemModel
{
    public:
      	RemoteButtonModel(QObject *parent=0);
	RemoteButtonModel(QList<RemoteControlButton> buttonList , QObject *parent=0);
	RemoteControlButton* getButton( int index ) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	void appendRow( RemoteControlButton *button);
	Qt::ItemFlags flags(const QModelIndex& index) const;
};


class RemoteModel : public QStringListModel
{
  public:
        RemoteModel(const QStringList &strings, QObject *parent = 0);
    	RemoteModel(QObject *parent=0);
         QVariant headerData(int section, Qt::Orientation o, int role) const;
};
#endif /* MODEL_H_ */

