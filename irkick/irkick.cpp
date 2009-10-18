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

// irkick.cpp  -  Implementation of the main window

#include "irkick.h"
#include "profileserver.h"
#include "irkickadaptor.h"

#include <QtDBus/qdbusconnection.h>

#include <kapplication.h>
#include <kactioncollection.h>
#include <ksystemtrayicon.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kdebug.h>
#include <klocale.h>
#include <kwindowsystem.h>
#include <kconfiggroup.h>
#include <knotification.h>
#include <ktoolinvocation.h>
#include <khelpmenu.h>

using namespace Solid::Control;

IRKick::IRKick():
        KNotificationItem(), npApp(QString())
{

    setIconByName("irkick");
    setCategory(Hardware);

    m_menu = new KMenu(associatedWidget());
    setContextMenu(m_menu);

    new IrkickAdaptor(this);
    QDBusConnection dBusConnection = QDBusConnection::sessionBus();
    dBusConnection.registerObject("/IRKick", this,
                                  QDBusConnection::ExportAllSlots);

    

    kDebug() << "loading solid";
                                  
    if (!Solid::Control::RemoteControlManager::connected()) {
        kDebug() << "Lirc not ready yet...";
    }
    theFlashOff = new QTimer(this);
    theFlashOff->setSingleShot(true);
    connect(theFlashOff, SIGNAL(timeout()), SLOT(flashOff()));

    theResetCount = 0;
    resetModes();
    slotReloadConfiguration();
    connect(RemoteControlManager::notifier(), SIGNAL(statusChanged(bool)), this, SLOT(slotStatusChanged(bool)));
    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc, 
                SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)), 
                this, 
                SLOT(gotMessage(const Solid::Control::RemoteControlButton &)));
    }

}

IRKick::~IRKick()
{
}

void IRKick::slotStatusChanged(bool connected)
{
  
    if(connected){
    KNotification::event("global_event", i18n("A connection to the infrared system has been made. Remote controls may now be available."),
                SmallIcon("irkick"), associatedWidget());
    updateTray();
    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc, 
                SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)), 
                this, 
                SLOT(gotMessage(const Solid::Control::RemoteControlButton &)));
    }
    resetModes();
    } else {
      KNotification::event("global_event", i18n("The infrared system has severed its connection. Remote controls are no longer available."), SmallIcon("irkick"), associatedWidget());
      updateTray();
    }
}

void IRKick::flashOff()
{
    setIconByName("irkick");
}

void IRKick::resetModes()
{
    kDebug()<< "resseting modes";
    if (theResetCount > 1) {
        KNotification::event("global_event", i18n("Resetting all modes."),
                             SmallIcon("irkick"), associatedWidget());
    }

    if (!theResetCount)
        allModes.generateNulls(Solid::Control::RemoteControl::allRemoteNames());

    foreach (const QString &remote, Solid::Control::RemoteControl::allRemoteNames()) {
        kDebug() << "adding remote" << remote << "to modes";
        currentModes[remote] = allModes.getDefault(remote).name();
    }
    updateContextMenu();
    updateTray();
    ++theResetCount;
}

void IRKick::slotReloadConfiguration()
{
    // load configuration from config file
    KConfig theConfig("irkickrc");
    allActions.loadFromConfig(theConfig);
    allModes.loadFromConfig(theConfig);
    if (currentModes.count() && theResetCount) {
        kDebug()<< "reloading conf";
        resetModes();
    }
}

void IRKick::slotConfigure()
{
    KToolInvocation::startServiceByDesktopName("kcm_lirc");
}

void IRKick::slotModeSelected(QAction *action)
{
    Mode mode = qVariantValue<Mode>(action->data());
    currentModes[mode.remote()] = mode.name();
    action->setChecked(true);
}

void IRKick::updateTray()
{
    QString toolTipHeader = i18n("KDE Lirc Server: ");
    QString toolTip;
    QString icon = "irkick";
    if (!Solid::Control::RemoteControlManager::connected()) {
        toolTipHeader += i18nc("The state of kdelirc", "Stopped");
        toolTip += i18n("Lirc daemon is currently not available.");
        icon = "irkickoff";
        setStatus(Passive);
    } else if (currentModes.size() == 0) {
        toolTipHeader += i18nc("The state of kdelirc", "Stopped");
        toolTip += i18n("No infra-red remote controls found.");
        setStatus(Passive);
    } else {
        toolTipHeader += i18nc("The state of kdelirc", "Ready");
        for (QMap<QString, QString>::const_iterator i = currentModes.constBegin(); i != currentModes.constEnd(); ++i) {
            Mode mode = allModes.getMode(i.key(), i.value());
            toolTip += mode.remote() + " <i>(";
            toolTip += mode.name().isEmpty() ? i18n("Master") : mode.name();
            toolTip +=")</i><br>";
        }
        setStatus(Active);
    }
    setToolTip("infrared-remote", toolTipHeader, toolTip);
    setIconByName(icon);
}

bool IRKick::searchForProgram(const IRAction &action, QStringList &programs)
{
    QDBusConnectionInterface *dBusIface =
        QDBusConnection::sessionBus().interface();
    programs.clear();

    if (action.unique()) {
	QString service = ProfileServer::getInstance()->getServiceName(action.program());
	if(service.isNull()){
	    service = action.program();
	}
	
	kDebug() << "searching for prog:" << service;
	if (dBusIface->isServiceRegistered(service)) {
	    kDebug() << "adding Program: " << service;
	    programs += service;
        } else {
	    kDebug() << "nope... " + service + " not here.";
        }
    } else {

        // find all instances...
        const QStringList buf = dBusIface->registeredServiceNames();

        for (QStringList::const_iterator i = buf.constBegin(); i != buf.constEnd(); ++i) {
            QString program = *i;
            if (program.contains(action.program()))
                programs += program;
        }

        if (programs.size() == 1) {
            kDebug() << "Yeah! found it!";
        } else if (programs.size() == 0) {
            kDebug() << "Nope... not here...";
        } else {
            kDebug() << "found multiple instances...";
        }

        if (programs.size() > 1 && action.ifMulti() == IM_DONTSEND) {
            kDebug() << "size:" << programs.size() << "ifmulti:" << action.ifMulti();
            return false;
        } else if (programs.size() > 1 && action.ifMulti() == IM_SENDTOTOP) {
            ;
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); i++) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action.program() + '-' + QString().setNum(p);
                if (programs.contains(id)) {
                    programs.clear();
                    programs += id;
                    break;
                }
            }
            while (programs.size() > 1) programs.removeFirst();
        } else if (programs.size() > 1 && action.ifMulti() == IM_SENDTOBOTTOM) {
            ;
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); ++i) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action.program() + '-' + QString().setNum(p);
                if (programs.contains(id)) {
                    programs.clear();
                    programs += id;
                    break;
                }
            }
            while (programs.size() > 1) programs.removeFirst();
        }
    }
    kDebug() << "returning true";
    return true;
}


void IRKick::executeAction(const IRAction& action) {
    kDebug() << "executeAction called with action:" << action.arguments().getArgumentsList();
    QDBusConnectionInterface *dBusIface =
        QDBusConnection::sessionBus().interface();

    QStringList programs;

    if (!searchForProgram(action, programs)) {
        return;
    }

    // if programs.size()==0 here, then the app is definately not running.
    kDebug() << "Autostart: " << action.autoStart();
    kDebug() << "programs.size: " << programs.size();
    if (action.autoStart() && !programs.size()) {
        kDebug() << "Should start " << action.program();
        QString sname = ProfileServer::getInstance()->getServiceName(
                            action.program());
        if (!sname.isNull()) {
            KNotification::event("app_event", i18n("Starting <b>%1</b>...",
                                                   action.application()), SmallIcon("irkick"));
            kDebug() << "starting service:" << action.program();
            QString error;
	    if (KToolInvocation::startServiceByDesktopName(action.program(), QString(), &error)) {
		kDebug() << "starting " + action.program() + " failed: " << error;
            }
        } else if (action.program().contains(QRegExp("org.[a-zA-Z0-9]*."))) {
            QString runCommand = action.program();
            runCommand.remove(QRegExp("org.[a-zA-Z0-9]*."));
            kDebug() << "runCommand" << runCommand;
            KToolInvocation::startServiceByDesktopName(runCommand);
        }
    }
    if (action.isJustStart())
        return;

    if (!searchForProgram(action, programs))
        return;

    for (QStringList::iterator i = programs.begin(); i != programs.end(); ++i) {
        const QString &program = *i;
        kDebug() << "Searching DBus for program:" << program;
        if (dBusIface->isServiceRegistered(program)) {
            kDebug() << "Sending data (" << program << ", " << '/' + action.object() << ", " << action.method().prototypeNR();

            QDBusMessage m = QDBusMessage::createMethodCall(program, '/'
                             + action.object(), "", action.method().prototypeNR());

            foreach(const QVariant &arg, action.arguments().getArgumentsList()){
                kDebug() << "Got argument:" << arg.type() << "value" << arg;
                m << arg;
            }
            //   theDC->send(program.utf8(), action.object().utf8(), action.method().prototypeNR().utf8(), data);
            QDBusMessage response = QDBusConnection::sessionBus().call(m);
            if (response.type() == QDBusMessage::ErrorMessage) {
                kDebug() << response.errorMessage();
            }
        }
    }
}

void IRKick::updateContextMenu(){
    m_menu->clear();
    m_menu->addTitle(KIcon("infrared-remote"), "IRKick");
    m_menu->addAction(SmallIcon("configure"), i18n("&Configure..."), this, SLOT(slotConfigure()));

    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        KMenu *modeMenu = new KMenu(remote, m_menu);
        QActionGroup *actionGroup = new QActionGroup(modeMenu);
        actionGroup->setExclusive(true);
        modeMenu->addTitle(KIcon("infrared-remote"), i18n("Switch mode to"));
        foreach(const Mode &mode, allModes.getModes(remote)){
            QAction *entry = modeMenu->addAction(mode.name().isEmpty() ? i18n("Master") : mode.name());
            entry->setActionGroup(actionGroup);
            entry->setCheckable(true);
            if(currentModes[remote] == mode.name()){
                entry->setChecked(true);
            }
            entry->setData(qVariantFromValue(mode));
        }
        m_menu->addMenu(modeMenu);
        connect(modeMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotModeSelected(QAction*)));

    }

    KHelpMenu *helpMenu = new  KHelpMenu(0, KGlobal::mainComponent().aboutData());
    m_menu->addAction(KIcon("help-contents"), i18n("&Help"), helpMenu, SLOT(appHelpActivated()));
    m_menu->addAction(KIcon("irkick"), i18n("&About"), helpMenu,SLOT(aboutApplication()));

    m_menu->addSeparator();
}

void IRKick::gotMessage(const RemoteControlButton &button)
{
    kDebug() << "Got message: " << button.remoteName() << ": " << button.name() << " (" << button.repeatCounter() << ")";
    if (!npApp.isEmpty()) {
        QString theApp = npApp;
        npApp.clear();
        // send notifier by DBUS to npApp/npModule/npMethod(theRemote, theButton);
        kDebug() << "Sending keypress to: " << theApp << ":" << npModule << ":" << npMethod;
        kDebug() << "Parameters: " << button.remoteName() << button.name();
        QDBusMessage m = QDBusMessage::createMethodCall(theApp, npModule, "",
                         npMethod);
        m << button.remoteName() << button.name();
        QDBusMessage response = QDBusConnection::sessionBus().call(m);
        if (response.type() == QDBusMessage::ErrorMessage) {
            kDebug() << response.errorMessage();
        }
    } else {
        if (currentModes[button.remoteName()].isNull()) {
            currentModes[button.remoteName()] = "";
        }
        kDebug() << "current mode:" << currentModes[button.remoteName()];
        IRActions l = allActions.findByModeButton(Mode(button.remoteName(), currentModes[button.remoteName()]), button.name());
        if (!currentModes[button.remoteName()].isEmpty())
            l += allActions.findByModeButton(Mode(button.remoteName(), ""), button.name());
        bool doBefore = true, doAfter = false;
        for (int i = 0; i < l.size(); ++i)
            if (l.at(i)->isModeChange() && !button.repeatCounter()) { // mode switch
                currentModes[button.remoteName()] = l.at(i)->modeChange();
                Mode mode = allModes.getMode(button.remoteName(), l.at(i)->modeChange());
                updateTray();
                doBefore = l.at(i)->doBefore();
                doAfter = l.at(i)->doAfter();
                KNotification::event(
                               "mode_event", "<b>" + mode.remote() + ":</b><br>" +
                               i18n("Mode switched to %1" , currentModes[button.remoteName()] == "" ? i18nc("Default mode in notification", "Default") : currentModes[button.remoteName()]),
                               DesktopIcon(mode.iconFile().isEmpty() ? "infrared-remote" : mode.iconFile()),
                               associatedWidget());
                break;
            }

        for (int after = 0; after < 2; after++) {
            if ((doBefore && !after) || (doAfter && after))
                for (int i = 0; i < l.size(); ++i) {
                    if (!l.at(i)->isModeChange() && (l.at(i)->repeat() || !button.repeatCounter())) {
                        executeAction(*l.at(i));
                    }
                }
            if (!after && doAfter) {
                l = allActions.findByModeButton(Mode(button.remoteName(), currentModes[button.remoteName()]), button.name());
                if (!currentModes[button.remoteName()].isEmpty()) {
                    l += allActions.findByModeButton(Mode(button.remoteName(), ""), button.name());
                }
            }
        }
    }
    setIconByName("irkickflash");
    theFlashOff->start(200);
}

const QStringList IRKick::buttons(QString theRemote) {
    QStringList retList;
    foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(theRemote).buttons()){
        retList.append(button.name());
    }
    return retList;
}

void IRKick::stealNextPress(QString app, QString module, QString method)
{
    npApp = app;
    npModule = module;
    npMethod = method;
}

void IRKick::dontStealNextPress()
{
    npApp.clear();
}

#include "irkick.moc"
