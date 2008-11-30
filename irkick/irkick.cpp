/***************************************************************************
                          irkick.cpp  -  Implementation of the main window
                             -------------------
    copyright            : (C) 2002 by Gav Wood
    email                : gav@kde.org
***************************************************************************/

// This program is free software.

#include <QWidget>
#include <qdialog.h>

#include <QRegExp>
#include <QTimer>
#include <qevent.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <kdeversion.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ksimpleconfig.h>
#include <ksystemtrayicon.h>
#include <kiconloader.h>
#include <kpassivepopup.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kdebug.h>
#include <klocale.h>
#include <kwindowsystem.h>
#include <kwinglobals.h>
#include <khelpmenu.h>
#include <kglobal.h>
#include <kstdguiitem.h>
#include <kconfiggroup.h>

#include <ktoolinvocation.h>

#include "profileserver.h"
#include "irkick.h"
#include "irkickadaptor.h"
#include <QtDBus/qdbusconnection.h>

IRKick::IRKick(const QString &obj)
    : QObject(), npApp(QString::null)	//krazy:exclude=nullstrassign for old broken gcc
{
	Q_UNUSED(obj)
	new IrkickAdaptor(this);
	QDBusConnection dBusConnection = QDBusConnection::sessionBus();
	dBusConnection.registerObject("/IRKick", this, QDBusConnection::ExportAllSlots);
	theClient = new KLircClient();

	theTrayIcon = new IRKTrayIcon();
	if(theClient->isConnected())
	{	theTrayIcon->setIcon(theTrayIcon->loadIcon("irkick"));
		theTrayIcon->setToolTip( i18n("KDE Lirc Server: Ready."));
	}
	else
	{	theTrayIcon->setIcon(theTrayIcon->loadIcon("irkickoff"));
		theTrayIcon->setToolTip( i18n("KDE Lirc Server: No infra-red remote controls found."));
		QTimer::singleShot(10000, this, SLOT(checkLirc()));
	}
	theFlashOff = new QTimer(theTrayIcon);
	connect(theFlashOff, SIGNAL(timeout()), SLOT(flashOff()));

	theResetCount = 0;
	slotReloadConfiguration();
	connect(theClient, SIGNAL(connectionClosed()), this, SLOT(slotClosed()));
	connect(theClient, SIGNAL(remotesRead()), this, SLOT(resetModes()));
	connect(theClient, SIGNAL(commandReceived(const QString &, const QString &, int)), this, SLOT(gotMessage(const QString &, const QString &, int)));

//FIXME: Bring back the Tray Icons Menu
	theTrayIcon->contextMenu()->setTitle( "IRKick");
	theTrayIcon->contextMenu()->addAction(SmallIcon( "configure" ), i18n("&Configure..."), this, SLOT(slotConfigure()));
//	QAction *helpAction = new QAction(SmallIcon( "help-contents" ), i18n("&Help"), );
//	theTrayIcon->contextMenu()->addAction(helpAction);
//	theTrayIcon->contextMenu()->insertSeparator();
	theTrayIcon->actionCollection()->action("file_quit")->disconnect(SIGNAL(activated()));
	connect(theTrayIcon->actionCollection()->action("file_quit"), SIGNAL(activated()), SLOT(doQuit()));

	theTrayIcon->show();
}

IRKick::~IRKick()
{
	delete theTrayIcon;
	for(QMap<QString,IRKTrayIcon *>::iterator i = currentModeIcons.begin(); i != currentModeIcons.end(); ++i)
		if(*i) delete *i;
}

void IRKick::slotClosed()
{
	theTrayIcon->loadIcon("irkickoff");
	KPassivePopup::message("IRKick", i18n("The infrared system has severed its connection. Remote controls are no longer available."), SmallIcon("irkick"), theTrayIcon);
	QTimer::singleShot(1000, this, SLOT(checkLirc()));
}

void IRKick::checkLirc()
{
	if(!theClient->isConnected()) {
		if(theClient->connectToLirc()) {
			KPassivePopup::message("IRKick", i18n("A connection to the infrared system has been made. Remote controls may now be available."), SmallIcon("irkick"), theTrayIcon);
			theTrayIcon->setIcon(theTrayIcon->loadIcon("irkick"));
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
	KConfig theConfig("irkickrc");
	KConfigGroup generalGroup = theConfig.group("General");
	switch(KMessageBox::questionYesNoCancel(0, i18n("Should the Infrared Remote Control server start automatically when you begin KDE?"), i18n("Automatically Start?"), KGuiItem(i18n("Start Automatically")), KGuiItem(i18n("Do Not Start"))))
	{	case KMessageBox::No: generalGroup.writeEntry("AutoStart", false); break;
		case KMessageBox::Yes: generalGroup.writeEntry("AutoStart", true); break;
		case KMessageBox::Cancel: return;
	}
	KApplication::kApplication()->quit();
}

void IRKick::resetModes()
{
	if(theResetCount > 1)
		KPassivePopup::message("IRKick", i18n("Resetting all modes."), SmallIcon("irkick"), theTrayIcon);
	if(!theResetCount)
		allModes.generateNulls(theClient->remotes());

	QStringList remotes = theClient->remotes();
	for(QStringList::iterator i = remotes.begin(); i != remotes.end(); ++i)
	{	currentModes[*i] = allModes.getDefault(*i).name();
		if(theResetCount && currentModeIcons[*i]) delete currentModeIcons[*i];
		currentModeIcons[*i] = 0;
	}
	updateModeIcons();
	theResetCount++;
}

void IRKick::slotReloadConfiguration()
{
	// load configuration from config file
	KConfig theConfig("irkickrc");
	allActions.loadFromConfig(theConfig);
	allModes.loadFromConfig(theConfig);
	if(currentModes.count() && theResetCount)
		resetModes();
}

void IRKick::slotConfigure()
{
	KToolInvocation::startServiceByDesktopName("kcmlirc");
}

void IRKick::updateModeIcons()
{
	for(QMap<QString,QString>::iterator i = currentModes.begin(); i != currentModes.end(); ++i)
	{
		Mode mode = allModes.getMode(i.key(), i.value());
		if(mode.iconFile().isNull() || mode.iconFile().isEmpty())
		{
			if(currentModeIcons[i.key()])
			{
				delete currentModeIcons[i.key()];
				currentModeIcons[i.key()] = 0;
			}
		}
		else
		{
			if(!currentModeIcons[i.key()])
			{
				currentModeIcons[i.key()] = new IRKTrayIcon();
				currentModeIcons[i.key()]->show();
//				currentModeIcons[i.key()]->contextMenu()->addTitle( mode.remoteName());
				currentModeIcons[i.key()]->actionCollection()->action("file_quit")->setEnabled(false);
			}
			currentModeIcons[i.key()]->loadIcon(mode.iconFile());
			currentModeIcons[i.key()]->setToolTip( mode.remoteName() + ": <b>" + mode.name() + "</b>");
		}
	}
}

bool IRKick::getPrograms(const IRAction &action, QStringList &programs)
{
	QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
	programs.clear();

	if(action.unique())
	{
		kDebug() << "searching for prog:" << action.program();
		if(dBusIface->isServiceRegistered(action.program())){
			kDebug() << "adding Program: " << action.program();
			programs += action.program();
		}
	}
	else
	{

		// find all instances...
		QStringList buf = dBusIface->registeredServiceNames();

		for(QStringList::iterator i = buf.begin(); i != buf.end(); ++i)
		{
			QString program = *i;
			if(program.contains(action.program()))
				programs += program;
		}

		if(programs.size() == 1){
			kDebug() << "Yeah! found it!";
		} else if(programs.size() == 0) {
			kDebug() << "Nope... not here...";
		} else {
			kDebug() << "found multiple instances...";
		}

		if(programs.size() > 1 && action.ifMulti() == IM_DONTSEND) {
			kDebug() << "size:" << programs.size() << "ifmulti:" << action.ifMulti();
			return false;
		} else if(programs.size() > 1 && action.ifMulti() == IM_SENDTOTOP){ ;
			QList<WId> s = KWindowSystem::stackingOrder();
			// go through all the (ordered) window pids
			for(int i = 0; i < s.size(); i++) {
				int p = KWindowSystem::windowInfo(s.at(i),NET::WMPid).win();
				QString id = action.program() + "-" + QString().setNum(p);
				if(programs.contains(id))
				{	programs.clear();
					programs += id;
					break;
				}
			}
			while(programs.size() > 1) programs.removeFirst();
		}
		else if(programs.size() > 1 && action.ifMulti() == IM_SENDTOBOTTOM){ ;
			QList<WId> s = KWindowSystem::stackingOrder();
			// go through all the (ordered) window pids
			for(int i = 0; i < s.size(); ++i) {
				int p = KWindowSystem::windowInfo(s.at(i),NET::WMPid).win();
				QString id = action.program() + "-" + QString().setNum(p);
				if(programs.contains(id)) {
					programs.clear();
					programs += id;
					break;
				}
			}
			while(programs.size() > 1) programs.removeFirst();
		}
	}
	kDebug() << "returning true";
	return true;
}

void IRKick::executeAction(const IRAction &action)
{
	kDebug() << "executeAction called";
	QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();

	QStringList programs;

	if(!getPrograms(action, programs)){
		return;
	}

	// if programs.size()==0 here, then the app is definately not running.
	kDebug() << "Autostart: " << action.autoStart();
	kDebug() << "programs.size: " << programs.size();
	if(action.autoStart() && !programs.size()) {
		kDebug() << "Should start " << action.program();
		QString sname = ProfileServer::profileServer()->getServiceName(action.program());
		if(!sname.isNull())
		{
			KPassivePopup::message("IRKick", i18n("Starting <b>%1</b>...", action.application()), SmallIcon("irkick"), theTrayIcon);
			kDebug() << "starting service:" << sname;
			KToolInvocation::startServiceByDesktopName(sname);
		} else if(action.program().contains(QRegExp("org.[a-zA-Z0-9]*."))){
			QString runCommand = action.program();
			runCommand.replace(QRegExp("org.[a-zA-Z0-9]*."), "");
			kDebug() << "runCommand" << runCommand;
			KToolInvocation::startServiceByDesktopName(runCommand);
		}
	}
	if(action.isJustStart()) return;

	if(!getPrograms(action, programs)) return;

	for(QStringList::iterator i = programs.begin(); i != programs.end(); ++i)
	{	const QString &program = *i;
		kDebug() << "Searching DBus for program:" << program;
		if(dBusIface->isServiceRegistered(program)){
			kDebug() << "Sending data (" << program << ", " << "/" + action.object() << ", " << action.method().prototypeNR() ;

			QDBusMessage m = QDBusMessage::createMethodCall(program, "/" + action.object(), "", action.method().prototypeNR());

			for(Arguments::const_iterator j = action.arguments().begin(); j != action.arguments().end(); ++j) {
				kDebug() << "Got argument:" << (*j).type() ;
//				m << (*j).toString();
				m << (*j);
			}
//			theDC->send(program.utf8(), action.object().utf8(), action.method().prototypeNR().utf8(), data);
			QDBusMessage response = QDBusConnection::sessionBus().call(m);
			if(response.type() == QDBusMessage::ErrorMessage){
				kDebug() << response.errorMessage();
			}
		}
	}
}

void IRKick::gotMessage(const QString &theRemote, const QString &theButton, int theRepeatCounter)
{
	kDebug() << "Got message: " << theRemote << ": " << theButton << " (" << theRepeatCounter << ")" ;
	theTrayIcon->setIcon(theTrayIcon->loadIcon("irkickflash"));
	theFlashOff->start(200);
	if(!npApp.isNull())
	{
		QString theApp = npApp;
		npApp = QString();
		// send notifier by DBUS to npApp/npModule/npMethod(theRemote, theButton);
		kDebug() << "Sending keypress to: " << theApp << ":" << npModule << ":" << npMethod;
		kDebug() << "Parameters: " << theRemote << theButton;
		QDBusMessage m = QDBusMessage::createMethodCall(theApp, npModule, "", npMethod);
		m << theRemote << theButton;
		QDBusMessage response = QDBusConnection::sessionBus().call(m);
		if(response.type() == QDBusMessage::ErrorMessage){
			kDebug() << response.errorMessage();
		}
	} else {
		if(currentModes[theRemote].isNull()) currentModes[theRemote] = "";
	kDebug() << "current mode:" << currentModes[theRemote];
		IRActions l = allActions.findByModeButton(Mode(theRemote, currentModes[theRemote]), theButton);
		if(!currentModes[theRemote].isEmpty())
			l += allActions.findByModeButton(Mode(theRemote, ""), theButton);
		bool doBefore = true, doAfter = false;
		for(int i = 0; i < l.size(); ++i)
			if(l.at(i)->isModeChange() && !theRepeatCounter)
			{	// mode switch
				currentModes[theRemote] = l.at(i)->modeChange();
				Mode mode = allModes.getMode(theRemote, l.at(i)->modeChange());
				updateModeIcons();
				doBefore = l.at(i)->doBefore();
				doAfter = l.at(i)->doAfter();
				KPassivePopup::message("IRKick", i18n("Mode switched to %1").arg(currentModes[theRemote] == "" ? i18n("Default") : currentModes[theRemote]), SmallIcon("irkick"), theTrayIcon);
				break;
			}

		for(int after = 0; after < 2; after++) {
			if((doBefore && !after) || (doAfter && after))
				for(int i = 0; i < l.size(); ++i) {
					if(!l.at(i)->isModeChange() && (l.at(i)->repeat() || !theRepeatCounter)) {
						executeAction(*l.at(i));
					}
				}
			if(!after && doAfter){
				l = allActions.findByModeButton(Mode(theRemote, currentModes[theRemote]), theButton);
				if(!currentModes[theRemote].isEmpty()) {
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
	npApp = QString();
}

#include "irkick.moc"
