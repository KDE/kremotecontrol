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

IRKick::IRKick():
        QObject(), npApp(QString())
{
    new IrkickAdaptor(this);
    QDBusConnection dBusConnection = QDBusConnection::sessionBus();
    dBusConnection.registerObject("/IRKick", this,
                                  QDBusConnection::ExportAllSlots);
    theClient = new KLircClient();

    theTrayIcon = new KSystemTrayIcon();
    if (! theClient->isConnected()) {
        QTimer::singleShot(10000, this, SLOT(checkLirc()));
    }
    theFlashOff = new QTimer(theTrayIcon);
    theFlashOff->setSingleShot(true);
    connect(theFlashOff, SIGNAL(timeout()), SLOT(flashOff()));

    theResetCount = 0;
    slotReloadConfiguration();
    connect(theClient, SIGNAL(connectionClosed()), this, SLOT(slotClosed()));
    connect(theClient, SIGNAL(remotesRead()), this, SLOT(resetModes()));
    connect(theClient, SIGNAL(commandReceived(const QString &, const QString &, int)), this, SLOT(gotMessage(const QString &, const QString &, int)));

    theTrayIcon->contextMenu()->setTitle("IRKick");
    theTrayIcon->contextMenu()->addAction(SmallIcon("configure"), i18n("&Configure..."), this, SLOT(slotConfigure()));
    KHelpMenu *helpMenu = new  KHelpMenu(0, KGlobal::mainComponent().aboutData());
    theTrayIcon->contextMenu()->addAction(KIcon("help-contents"), i18n("&Help"), helpMenu, SLOT(appHelpActivated()));
    theTrayIcon->contextMenu()->addAction(KIcon("irkick"), i18n("&About"), helpMenu,SLOT(aboutApplication()));

    theTrayIcon->contextMenu()->addSeparator();
    theTrayIcon->actionCollection()->action("file_quit")->disconnect();
    connect(theTrayIcon->actionCollection()->action("file_quit"), SIGNAL(activated()), SLOT(doQuit()));
    theTrayIcon->show();
    updateTray();
}

IRKick::~IRKick()
{
    delete theTrayIcon;
}

void IRKick::slotClosed()
{
    KNotification::event("global_event", i18n("The infrared system has severed its connection. Remote controls are no longer available."), SmallIcon("irkick"), theTrayIcon->parentWidget());
    QTimer::singleShot(1000, this, SLOT(checkLirc()));
    updateTray();
}

void IRKick::checkLirc()
{
    if (!theClient->isConnected()) {
        if (theClient->connectToLirc()) {
            KNotification::event("global_event", i18n("A connection to the infrared system has been made. Remote controls may now be available."),
                                 SmallIcon("irkick"), theTrayIcon->parentWidget());
            updateTray();
        } else {
            QTimer::singleShot(10000, this, SLOT(checkLirc()));
        }
    }
}

void IRKick::flashOff()
{
    theTrayIcon->setIcon(theTrayIcon->loadIcon("irkick"));
}

void IRKick::doQuit()
{
    kDebug() << "doQuit called";
    KConfig theConfig("irkickrc");
    KConfigGroup generalGroup = theConfig.group("General");
    switch (KMessageBox::questionYesNoCancel(
                0,
                i18n(
                    "Should the Infrared Remote Control server start automatically when you begin KDE?"),
                i18n("Automatically Start?"), KGuiItem(i18n("Start Automatically")),
                KGuiItem(i18n("Do Not Start")))) {
    case KMessageBox::No:
        generalGroup.writeEntry("AutoStart", false);
        break;
    case KMessageBox::Yes:
        generalGroup.writeEntry("AutoStart", true);
        break;
    case KMessageBox::Cancel:
        return;
    }
    KApplication::kApplication()->quit();
}

void IRKick::resetModes()
{
    kDebug()<< "resseting modes";
    if (theResetCount > 1) {
        KNotification::event("global_event", i18n("Resetting all modes."),
                             SmallIcon("irkick"), theTrayIcon->parentWidget());
    }

    if (!theResetCount)
        allModes.generateNulls(theClient->remotes());

    QStringList remotes = theClient->remotes();
    for (QStringList::iterator i = remotes.begin(); i != remotes.end(); ++i) {
        currentModes[*i] = allModes.getDefault(*i).name();
    }
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

void IRKick::updateTray()
{
    QString toolTip="<qt><nobr>";
    QString icon = QString("irkick");
    if (!theClient->isConnected()) {
        toolTip += i18n("Lirc daemon is currently not available.");
        toolTip+="</nobr";
        icon = "irkickoff";
    } else if (currentModes.size() == 0) {
        toolTip += i18n("KDE Lirc Server: No infra-red remote controls found.");
        toolTip+="</nobr";
    } else {
        toolTip+="<nobr><b><u>";
        toolTip += i18n("KDE Lirc Server: Ready.");
        toolTip+="</u></b></nobr>";
        for (QMap<QString, QString>::iterator i = currentModes.begin(); i != currentModes.end(); ++i) {
            Mode mode = allModes.getMode(i.key(), i.value());
            toolTip+="<br><nobr>";
            if ( !mode.iconFile().isEmpty()) {
                QString iconPath = KIconLoader::global()->iconPath( mode.iconFile(), KIconLoader::Small,false );
                toolTip += QString(" <img src=\"%1\"></img> ").arg(iconPath);
            }
            toolTip += "<b>"+ mode.remoteName() + "</b> <i>(";
            toolTip += mode.name().isEmpty() ? i18n("Master") : mode.name();
            toolTip +=")</i></nobr></br>";
        }
        toolTip+="</qt>";
    }
    theTrayIcon->setToolTip(toolTip);
    theTrayIcon->setIcon(theTrayIcon->loadIcon(icon));
}

bool IRKick::searchForProgram(const IRAction &action, QStringList &programs)
{
    QDBusConnectionInterface *dBusIface =
        QDBusConnection::sessionBus().interface();
    programs.clear();

    if (action.unique()) {
	QString service = ProfileServer::profileServer()->getServiceName(action.program());
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
        QStringList buf = dBusIface->registeredServiceNames();

        for (QStringList::iterator i = buf.begin(); i != buf.end(); ++i) {
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
        QString sname = ProfileServer::profileServer()->getServiceName(
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

void IRKick::gotMessage(const QString &theRemote, const QString &theButton,
                        int theRepeatCounter)
{
    kDebug() << "Got message: " << theRemote << ": " << theButton << " (" << theRepeatCounter << ")";
    theTrayIcon->setIcon(theTrayIcon->loadIcon("irkickflash"));
    theFlashOff->start(200);
    if (!npApp.isEmpty()) {
        QString theApp = npApp;
        npApp.clear();
        // send notifier by DBUS to npApp/npModule/npMethod(theRemote, theButton);
        kDebug() << "Sending keypress to: " << theApp << ":" << npModule << ":" << npMethod;
        kDebug() << "Parameters: " << theRemote << theButton;
        QDBusMessage m = QDBusMessage::createMethodCall(theApp, npModule, "",
                         npMethod);
        m << theRemote << theButton;
        QDBusMessage response = QDBusConnection::sessionBus().call(m);
        if (response.type() == QDBusMessage::ErrorMessage) {
            kDebug() << response.errorMessage();
        }
    } else {
        if (currentModes[theRemote].isNull()) currentModes[theRemote] = "";
        kDebug() << "current mode:" << currentModes[theRemote];
        IRActions l = allActions.findByModeButton(Mode(theRemote, currentModes[theRemote]), theButton);
        if (!currentModes[theRemote].isEmpty())
            l += allActions.findByModeButton(Mode(theRemote, ""), theButton);
        bool doBefore = true, doAfter = false;
        for (int i = 0; i < l.size(); ++i)
            if (l.at(i)->isModeChange() && !theRepeatCounter) { // mode switch
                currentModes[theRemote] = l.at(i)->modeChange();
                Mode mode = allModes.getMode(theRemote, l.at(i)->modeChange());
                updateTray();
                doBefore = l.at(i)->doBefore();
                doAfter = l.at(i)->doAfter();
                KNotification::event(
                               "mode_event",
                               i18n("Mode switched to %1", currentModes[theRemote] == "" ? i18nc("Default mode in notification", "Default") : currentModes[theRemote]),
                               DesktopIcon(mode.iconFile().isEmpty() ? "infrared-remote" : mode.iconFile()),
                               theTrayIcon->parentWidget());
                break;
            }

        for (int after = 0; after < 2; after++) {
            if ((doBefore && !after) || (doAfter && after))
                for (int i = 0; i < l.size(); ++i) {
                    if (!l.at(i)->isModeChange() && (l.at(i)->repeat() || !theRepeatCounter)) {
                        executeAction(*l.at(i));
                    }
                }
            if (!after && doAfter) {
                l = allActions.findByModeButton(Mode(theRemote, currentModes[theRemote]), theButton);
                if (!currentModes[theRemote].isEmpty()) {
                    l += allActions.findByModeButton(Mode(theRemote, ""), theButton);
                }
            }
        }
    }
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
