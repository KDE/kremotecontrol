
/*************************************************************************
 * Copyright: (C) 2009 by Frank Scheffold <fscheffold@googlemail.com>    *
 * Copyright: (C) 2009 by Michael Zanetti <michael_zanetti@gmx.net>      *
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

/**
 *
 *  Created on: 01.02.2009
 *      @author Frank Scheffold, Michael Zanetti
 */

#ifndef MODEL_H
#define MODEL_H


#include "prototype.h"
#include "profileaction.h"
#include "profile.h"
#include "remotelist.h"

#include <QItemDelegate>
#include <QStandardItem>
#include <solid/control/remotecontrol.h>
#include <QStringListModel>

using namespace Solid::Control;


Q_DECLARE_METATYPE(ProfileActionTemplate*)
Q_DECLARE_METATYPE(RemoteControlButton*)

class DBusServiceModel: public QStandardItemModel
{
    public:
        DBusServiceModel(QObject* parent = 0);
        QString application(const QModelIndex &index) const;
        QString node(const QModelIndex &index) const;
};

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
    
    void refresh(const QString &app, const QString &node);

    Prototype getPrototype( int index ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    void appendRow(Prototype item);

};


class ArgumentsModel: public QStandardItemModel
{
public:
    ArgumentsModel(QObject *parent = 0);
    
    void refresh(const Prototype &prototype);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    
    QList<Argument> arguments() const;
};

class ArgumentsModelItem: public QStandardItem
{
public:
    ArgumentsModelItem(const QString &text);
    ArgumentsModelItem(const Argument &arg);
    virtual QVariant data(int role = Qt::UserRole + 1) const;

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
    explicit ProfileModel(Profile *profile, QObject *parent=0);

    ProfileActionTemplate* getProfileActionTemlate(int index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void appendRow(ProfileActionTemplate actionTemplate);
    Qt::ItemFlags flags(const QModelIndex& index) const;
};


class RemoteButtonModel: public QStandardItemModel
{
private:
    QList<RemoteControlButton> m_buttonList;
public:
    RemoteButtonModel(QObject *parent=0);
    explicit RemoteButtonModel(const QList<RemoteControlButton> &buttonList , QObject *parent=0);
    RemoteControlButton* getButton( int index ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void appendRow( const RemoteControlButton &button);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QModelIndex indexOfButtonName(const QString &button);
};


class RemoteModel: public QStandardItemModel
{
    public:
        explicit RemoteModel(const RemoteList &remoteList, QObject *parent = 0);
        RemoteModel(QObject *parent=0);
        void refresh(const RemoteList &remoteList);
};

class RemoteItem: public QStandardItem
{
    public:
        RemoteItem(Remote *remote);
        virtual QVariant data(int role) const;
};

#endif /* MODEL_H_ */

