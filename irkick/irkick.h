/*************************************************************************
 * Copyright            : (C) 2002 by Gav Wood <gav@kde.org>             *
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

#ifndef IRKICK_H
#define IRKICK_H

#include "modes.h"
#include "iractions.h"
//#include "klircclient.h"

#include <knotificationitem.h>
#include <kaboutdata.h>

#include <solid/control/remotecontrolmanager.h>
#include <solid/control/remotecontrol.h>

class QTimer;
class QAction;

class IRKick: public KNotificationItem
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.irkick")


    QString npApp, npModule, npMethod;
    QMap<QString, QString> currentModes;
    IRActions allActions;
    int theResetCount;
    Modes allModes;
    KMenu *m_menu;
    KAboutData *aboutData;
    QTimer *theFlashOff;

    void updateTray();
    bool searchForProgram(const IRAction &action, QStringList &programs);
    void executeAction(const IRAction &action);
    void updateContextMenu();

public Q_SLOTS: //dbus slot
    /**
     * Query status of connection.
     *
     * @returns true if connected to lircd.
     */
    bool isConnected() {
        return Solid::Control::RemoteControlManager::connected();
    }

    /**
     * Retrieve list of remote controls.
     *
     * @returns said list.
     */
    const QStringList remotes() {
        return Solid::Control::RemoteControl::allRemoteNames();
    }

    /**
     * Retrieve list of buttons of a praticular remote control.
     *
     * @returns said list.
     */
    const QStringList buttons(QString theRemote); 

    /**
     * Sends next keypress to given application by DBUS.
     *
     * @param The receiving application.
     * @param The receiving application module.
     * @param The method name. Must have two QString parameters.
     */
    void stealNextPress(QString app, QString module, QString method);

    /**
     * Cancels the proceedings of the previous stealNextPress call, if any.
     */
    void dontStealNextPress();

    /**
     * Reloads configuration from file(s)
     */
    void reloadConfiguration() {
        slotReloadConfiguration();
    }

private slots:
    void gotMessage(const Solid::Control::RemoteControlButton &button);
    void resetModes();
    void flashOff();

    void slotConfigure();
    void slotReloadConfiguration();
    void slotStatusChanged(bool connected);
    void slotModeSelected(QAction *action);

public:
    explicit IRKick();
    virtual ~IRKick();
};

#endif
