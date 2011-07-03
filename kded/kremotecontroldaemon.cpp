/*
    Copyright (C) <2010>  <Frank Scheffold (fscheffold@googlemail.com)>

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

/**
  @author: Frank Scheffold
*/

#include "kremotecontroldaemon.h"
#include "krcdadaptor.h"

#include <dbusinterface.h>
#include <mode.h>
#include <action.h>
#include <executionengine.h>

#include <KCModuleInfo>
#include <KDebug>
#include <KNotification>
#include <KAboutData>
#include <KIconLoader>
#include <KToolInvocation>

#include <QtGui/QPixmap>
#include <kremotecontrol/libkremotecontrol/action.h>


K_PLUGIN_FACTORY(KRemoteControlDaemonFactory, registerPlugin<KRemoteControlDaemon>();)
K_EXPORT_PLUGIN(KRemoteControlDaemonFactory("kremotecontroldaemon"))

KRemoteControlDaemon::KRemoteControlDaemon(QObject* parent, const QVariantList& ): KDEDModule(parent) {

    new KrcdAdaptor(this);
    KAboutData aboutData("kremotecontroldaemon", "kremotecontroldaemon", ki18n("K Remote Control Daemon"),
                          "0.4", ki18n("Remote Control Daemon for KDE4"),
                          KAboutData::License_GPL, ki18n("(c) 2010 Frank Scheffold"),
                          KLocalizedString(), "http://www.kde.org");

    aboutData.addAuthor(ki18n("Michael Zanetti"), ki18n("Maintainer"), "michal_zanetti@gmx.net");
    aboutData.addAuthor(ki18n("Frank Scheffold"), ki18n("Developer"), "fscheffold@googlemail.com");
    aboutData.addCredit(ki18n("Gav Wood"), ki18n("Original KDELirc Developer"), "gav.wood@kde.org");

    m_applicationData = KComponentData(aboutData);

    connect(RemoteControlManager::notifier(), SIGNAL(statusChanged(bool)), this, SLOT(slotStatusChanged(bool)));
    connect(RemoteControlManager::notifier(), SIGNAL(remoteControlAdded(const QString&)), this, SLOT(slotRemoteControlAdded(const QString&)));
    connect(RemoteControlManager::notifier(), SIGNAL(remoteControlRemoved(const QString&)), this, SLOT(slotRemoteControlRemoved(const QString&)));

    m_remoteList.loadFromConfig(QLatin1String( "kremotecontrolrc" ));
    KConfig config(QLatin1String( "kremotecontrolrc" ));
    KConfigGroup globalGroup(&config, "Global");
    if(globalGroup.readEntry("ShowTrayIcon", false)){
        kDebug() << "starting notifier item";
        KToolInvocation::kdeinitExec(QLatin1String( "krcdnotifieritem" ));
    } else {
        emit unloadTray();
    }

    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc, SIGNAL(buttonPressed(const RemoteControlButton &)),
                this,  SLOT(gotMessage(const RemoteControlButton &)));
    }
    
    m_modeSwitchTimer.setSingleShot(true);
}

KRemoteControlDaemon::~KRemoteControlDaemon() {
    emit unloadTray();
}

void KRemoteControlDaemon::slotStatusChanged(bool connected) {
    if(connected){
        foreach(const QString &remote, RemoteControl::allRemoteNames()){
            RemoteControl *rc = new RemoteControl(remote);
            kDebug() << "connecting to remote" << remote;
            connect(rc,
                    SIGNAL(buttonPressed(const RemoteControlButton &)),
                    this,
                    SLOT(gotMessage(const RemoteControlButton &)));
        }
    }
    emit connectionChanged(connected);
}

void KRemoteControlDaemon::gotMessage(const RemoteControlButton& button) {
    kDebug()<< "Got message from remote " << button.remoteName() << " button " << button.name() << "repeat" << button.repeatCounter();
    Remote *remote = m_remoteList.remote(button.remoteName());
    if(!remote){
        kDebug()<< "No remote found for remote" << button.remoteName();
        return;
    }

    if(eventsIgnored(remote->name())){
        kDebug() << "Events for  "<< remote->name() <<    " are currently ignored";
        return;
    }

    emit(buttonPressed());

    if(remote->currentMode()){
        QVector<Action*> actionList;
        if(remote->currentMode() != remote->masterMode()){
            actionList += remote->masterMode()->actionsForButton(button.name());
        }
        actionList += remote->currentMode()->actionsForButton(button.name());
        if(button.repeatCounter() == 0 && remote->nextMode(button.name())){
            Mode *mode = remote->currentMode();
            notifyModeChanged(remote);
            if(remote->currentMode()-> doAfter()){
                actionList += remote->currentMode()->actionsForButton(button.name());
            }
            emit(modeChanged(remote->name(), mode->name()));
        }
        foreach(Action *action, actionList){
            if(action->repeat() || (button.repeatCounter() == 0)) {
                kDebug() << "executing " << action->name() << action->description() << "repeat" << action->repeat();
                ExecutionEngine::executeAction(action);
            } else {
                kDebug() << "not executing because of repeatblock. repeat:" << action->repeat() << "counter:" << button.repeatCounter();
            }
        }
    }
}

void KRemoteControlDaemon::reloadConfiguration() {
    foreach(Remote *remote, m_remoteList){
        delete remote;
    }
    m_remoteList.clear();
    m_remoteList.loadFromConfig(QLatin1String( "kremotecontrolrc" ));
    KConfig config(QLatin1String( "kremotecontrolrc" ));
    KConfigGroup globalGroup(&config, "Global");

    if(globalGroup.readEntry("ShowTrayIcon", false)){
        kDebug() << "starting notifier item";
        KToolInvocation::kdeinitExec(QLatin1String( "krcdnotifieritem" ));
    } else {
        emit unloadTray();
    }

    notifyEvent(i18n("Configuration reloaded."));
    foreach(const Remote *remote, m_remoteList){
        emit modeChanged(remote->name(), remote->masterMode()->name());
    }
}

void KRemoteControlDaemon::changeMode(const QString& remoteName, Mode* mode) {
    Remote *remote = m_remoteList.remote(remoteName);
    if(remote && remote->allModes().contains(mode)){
        remote->setCurrentMode(mode);
    }
}

void KRemoteControlDaemon::ignoreButtonEvents(const QString& remoteName) {
    if(remoteName.isEmpty()){
        foreach(const Remote *remote, m_remoteList){
            kDebug() << "muting remote" << remote->name();
            m_ignoreNextButtonList.append(remote->name());
        }
        m_ignoreNextButtonList.removeDuplicates();
    } else {
        kDebug() << "muting remote" << remoteName;
        m_ignoreNextButtonList.append(remoteName);
        m_ignoreNextButtonList.removeDuplicates();
    }
}

void KRemoteControlDaemon::considerButtonEvents(const QString& remoteName) {
    if(remoteName.isEmpty()){
        m_remoteList.clear();
    } else {
        foreach(const Remote *remote, m_remoteList){
            kDebug() << "unmuting remote" << remote->name();
            m_ignoreNextButtonList.removeAll(remote->name());
        }
    }
}

void KRemoteControlDaemon::slotRemoteControlAdded(const QString& name) {
    if(m_remoteList.remote(name)){
        kDebug() << "remote found";
        notifyEvent(i18n("The remote control %1 is now available.", name));
    }else{
        kDebug() << "remote not found";
        KNotification *notification = KNotification::event(QLatin1String( "global_event" ), i18n("An unconfigured remote control %1 is now available.", name),
                  DesktopIcon(QLatin1String( "infrared-remote" )), 0, KNotification::Persistant, m_applicationData);
        notification->setActions(QStringList() << i18nc("Configure the remote", "Configure remote"));
        connect(notification, SIGNAL(activated(unsigned int)), SLOT(lauchKcmShell()));
    }
    emit remoteControlAdded(name);
}

void KRemoteControlDaemon::lauchKcmShell() {
    kDebug() << "Launch kcmshell";
    KToolInvocation::startServiceByDesktopName(QLatin1String( "kcm_remotecontrol" ));
}

void KRemoteControlDaemon::slotRemoteControlRemoved(const QString& name) {
    notifyEvent(i18n("The remote %1 was removed from system.", name));
    emit remoteControlRemoved(name);
}

bool KRemoteControlDaemon::changeMode(const QString& remoteName, const QString& modeName) {
    Remote *remote = m_remoteList.remote(remoteName);
    if(remote){
        foreach(Mode *mode, remote-> allModes()){
            if(mode->name() == modeName){
                remote->setCurrentMode(mode);
                notifyModeChanged(remote);
                return true;
            }
        }
    }
    return false;
}

QStringList KRemoteControlDaemon::modesForRemote(const QString& remoteName) {
    QStringList list;
    Remote *remote = m_remoteList.remote(remoteName);
    if(remote){
        foreach(const Mode *mode, remote->allModes()){
            list << mode->name();
        }
    }
    return list;
}

QStringList KRemoteControlDaemon::configuredRemotes() {
    QStringList list;
    foreach(Remote *remote, m_remoteList){
        list << remote->name();
    }
    return list;
}

void KRemoteControlDaemon::notifyModeChanged(Remote* remote) {
    if(m_notification) {
        m_notification->setText(QLatin1String( "<b>" ) + remote->name() + QLatin1String( ":</b><br>" ) + i18n("Mode switched to %1" , remote->currentMode()->name()));
        m_notification->setPixmap(DesktopIcon(remote->currentMode()->iconName().isEmpty() ? QLatin1String( "infrared-remote" ) : remote->currentMode()->iconName()));
        m_notification->update();
        m_modeSwitchTimer.start(5000);
    } else {
        m_notification = KNotification::event(QLatin1String( "mode_event" ),
        QLatin1String( "<b>" ) + remote->name() + QLatin1String( ":</b><br>" ) + i18n("Mode switched to %1" , remote->currentMode()->name()),
        DesktopIcon(remote->currentMode()->iconName().isEmpty() ? QLatin1String( "infrared-remote" ) : remote->currentMode()->iconName())
        , 0, KNotification::Persistent, m_applicationData);
        m_modeSwitchTimer.start(5000);
        connect(&m_modeSwitchTimer, SIGNAL(timeout()), m_notification, SLOT(close()));
    }
}

QString KRemoteControlDaemon::currentMode(const QString& remoteName) {
    Remote *remote = m_remoteList.remote(remoteName);
    if(remote){
        return remote->currentMode()->name();
    }
    return QLatin1String( "modeNotFound" );
}

QString KRemoteControlDaemon::modeIcon(const QString &remoteName, const QString& modeName) {
    Remote *remote = m_remoteList.remote(remoteName);
    if(remote){
        Mode *mode = remote->modeByName(modeName);
        if(mode){
            return mode->iconName();
        }
    }
    return QString();
}

void KRemoteControlDaemon::notifyEvent(const QString& message, const QString& iconName, const QString& event) {
    KNotification::event(event, message, DesktopIcon(iconName), 0, KNotification::CloseOnTimeout, m_applicationData);
}

bool KRemoteControlDaemon::eventsIgnored(const QString& remoteName) {
    return m_ignoreNextButtonList.contains(remoteName);
}
