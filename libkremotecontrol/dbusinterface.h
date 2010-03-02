/*************************************************************************
 * Copyright: (C) 2009 Michael Zanetti <michael_zanetti@gmx.net>         *
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
 * dbusinterface.h
 *
 *  Created on: 14.02.2009
 *      Author: Michael Zanetti
 */

#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include "dbusaction.h"
#include <QStringList>

class KREMOTECONTROL_EXPORT DBusInterface: public QObject
{
    Q_OBJECT

    private:
        DBusInterface();

        QStringList getAllRegisteredPrograms();
        bool searchForProgram(const DBusAction *action, QStringList &programs);
    public:

        static DBusInterface *getInstance();
        ~DBusInterface();

        bool isProgramRunning(const QString &program);
        bool isUnique(const QString &program);

        QStringList getRegisteredPrograms();
        QStringList getNodes(const QString &program);
        QList<Prototype> getFunctions(const QString &program, const QString &object);
	
        QStringList getConfiguredRemotes();
        void ignoreButtonEvents(const QString &remoteName);
        void considerButtonEvents(const QString &remoteName);	
        void reloadRemoteControlDaemon();

        void executeAction(const DBusAction *action);
        QStringList getModesForRemote(const QString &remoteName);
        void changeMode(const QString &remoteName, const QString &modeName);
        QString getCurrentMode(const QString &remoteName);
        QString getModeIcon(const QString &remoteName, const QString &modeName);
        bool eventsIgnored(const QString &remoteName);
        
        bool isKdedModuleRunning();
        
        /**
          * Loads the kremotecontrol kded module and sets autoLoading to true
          */
        bool loadKdedModule();

        /**
          * Unloads the kremotecontrol kded module and sets autoLoading to false
          */
        bool unloadKdedModule();
};

#endif
