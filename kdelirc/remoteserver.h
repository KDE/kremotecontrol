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


/**
  * @author Gav Wood
  */

#ifndef REMOTESERVER_H
#define REMOTESERVER_H


#include <qxml.h>
#include <QHash>

#include <kdebug.h>

class RemoteButton
{
    QString theName, theId, theClass, theParameter;

    friend class Remote;
public:
    void setName(const QString &a) {
        theName = a;
    }
    const QString &name(void) const {
        return theName;
    }
    void setClass(const QString &a) {
        theClass = a;
    }
    const QString &getClass(void) const {
        return theClass;
    }
    void setParameter(const QString &a) {
        theParameter = a;
    }
    const QString &parameter(void) const {
        return theParameter;
    }
    void setId(const QString &a) {
        theId = a;
    }
    const QString &id(void) const {
        return theId;
    }
};

class Remote : public QXmlDefaultHandler
{
    QString theName, theId, theAuthor;
    QHash<QString, RemoteButton*> theButtons;

    QString charBuffer;
    RemoteButton *curRB;

    friend class RemoteServer;
public:
    bool characters(const QString &data);
    bool startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes);
    bool endElement(const QString &, const QString &, const QString &name);

    void setName(const QString &a) {
        theName = a;
    }
    const QString &name(void) const {
        return theName;
    }
    void setId(const QString &a) {
        theId = a;
    }
    const QString &id(void) const {
        return theId;
    }
    void setAuthor(const QString &a) {
        theAuthor = a;
    }
    const QString &author(void) const {
        return theAuthor;
    }
    const QHash<QString, RemoteButton*> &buttons() const {
        return theButtons;
    }

    void loadFromFile(const QString &fileName);

    const QString &getButtonName(const QString &id) const {
        if (theButtons[id]) return theButtons[id]->name(); return id;
    }

    Remote();
    ~Remote();
};

class RemoteServer
{
    static RemoteServer *theInstance;
    void loadRemotes();
    QHash<QString, Remote *> theRemotes;

public:
    static RemoteServer *remoteServer() {
        if (!theInstance) theInstance = new RemoteServer(); return theInstance;
    }

    const QHash<QString, Remote*> &remotes() const {
        return theRemotes;
    }

    const QString &getRemoteName(const QString &id) const {
        kDebug() << "Searching for Remote id: " << id;
        if (theRemotes[id])
            return theRemotes[id]->name();
        return id;
    }

    const QString &getButtonName(const QString &remote, const QString &button) const {
        if (theRemotes[remote]) return theRemotes[remote]->getButtonName(button); return button;
    }

    RemoteServer();
    ~RemoteServer();
};

#endif
