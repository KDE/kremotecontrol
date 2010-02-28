/*
    Copyright (C) 2010 Michael Zanetti <michael_zanetti@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "krcdnotifieritem.h"
#include "dbusinterface.h"

#include <solid/control/remotecontrolmanager.h>
#include <klocalizedstring.h>
#include <kicon.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>
#include <kdebug.h>

#include <QTimer>
#include <QDBusConnection>

KrcdNotifierItem::KrcdNotifierItem(){
    m_notifier.setCategory(KStatusNotifierItem::Hardware);
    updateTray();
    setContextMenu(&m_menu);
    
    // No need for close button...
    setStandardActionsEnabled(false); 
    updateContextMenu();
    QDBusConnection::sessionBus().connect("org.kde.kded", "/modules/kremotecontrol", "org.kde.krcd", "buttonPressed",  this, SLOT(flash()));
}

void KrcdNotifierItem::updateTray() {
    QString toolTipHeader = i18n("Remote Controls\n");
    QString toolTip;
    QString icon = "krcd";
    if (!Solid::Control::RemoteControlManager::connected()) {
        toolTipHeader += i18nc("The state of kremotecontrol", "Stopped");
        toolTip += i18n("No Remote Control Backend is currently available.");
        icon = "krcd_off";
        setStatus(Passive);
    } else {
        toolTipHeader += i18nc("The state of kremotecontrol", "Ready");
        foreach(const QString &remote, DBusInterface::getInstance()->getConfiguredRemotes()) {
            QString mode = DBusInterface::getInstance()->getCurrentMode(remote);
            toolTip += remote + " <i>(" + mode + ")</i><br>";
        }
        setStatus(Active);
    }
    setToolTip("infrared-remote", toolTipHeader, toolTip);
    setIconByName(icon);
}

void KrcdNotifierItem::updateContextMenu(){
    m_menu.clear();
    m_menu.addTitle(KIcon("infrared-remote"), "Remote Controls");
    m_menu.addAction(SmallIcon("configure"), i18n("&Configure..."), this, SLOT(slotConfigure()));

    foreach(const QString &remote, Solid::Control::RemoteControl::allRemoteNames()){
        KMenu *modeMenu = new KMenu(remote, &m_menu);
        QActionGroup *actionGroup = new QActionGroup(modeMenu);
        actionGroup->setExclusive(true);
        modeMenu->addTitle(KIcon("infrared-remote"), i18n("Switch mode to"));
        foreach(const QString &mode, DBusInterface::getInstance()->getModesForRemote(remote)){
            QAction *entry = modeMenu->addAction(mode);
            entry->setActionGroup(actionGroup);
            entry->setCheckable(true);
            if(DBusInterface::getInstance()->getCurrentMode(remote) == mode){
                entry->setChecked(true);
            }
            entry->setData(QStringList() << remote << mode);
        }
        m_menu.addMenu(modeMenu);
        connect(modeMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotModeSelected(QAction*)));

    }
}

void KrcdNotifierItem::slotConfigure() {
    KToolInvocation::startServiceByDesktopName("kcm_remotecontrol");
}

void KrcdNotifierItem::slotModeSelected ( QAction* action ) {
    QString remote = action->data().toStringList().first();
    QString mode = action->data().toStringList().last();
    DBusInterface::getInstance()->changeMode(remote, mode);
    action->setChecked(true);
    updateTray();
}

void KrcdNotifierItem::flash() {
    setIconByName("krcd_flash");
    QTimer::singleShot(200, this, SLOT(flashOff()));
}

void KrcdNotifierItem::flashOff() {
    setIconByName("krcd");
}
