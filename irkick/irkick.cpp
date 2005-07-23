/***************************************************************************
                          irkick.cpp  -  Implementation of the main window
                             -------------------
    copyright            : (C) 2002 by Gav Wood
    email                : gav@kde.org
***************************************************************************/

// This program is free software.

#include <qwidget.h>
#include <qdialog.h>
#include <qtooltip.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qevent.h>

#include <kdeversion.h>
#include <kapplication.h>
#include <kaction.h>
#if !(KDE_VERSION_MINOR<=1 && KDE_VERSION_RELEASE<=5)
#include <kactioncollection.h>
#endif
#include <ksimpleconfig.h>
#include <ksystemtray.h>
#include <kiconloader.h>
#include <kpassivepopup.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kdebug.h>
#include <klocale.h>
#include <kaboutdialog.h>
#include <kaboutkde.h>
#include <kwinmodule.h>
#include <kwin.h>
#include <khelpmenu.h>
#include <kglobal.h>
#include <kstdguiitem.h>

#include <dcopclient.h>
#include <dcopref.h>

#include "profileserver.h"
#include "irkick.h"

void IRKTrayIcon::mousePressEvent(QMouseEvent *e)
{
	KSystemTray::mousePressEvent(new QMouseEvent(QEvent::MouseButtonPress, e->pos(), e->globalPos(), e->button() == LeftButton ? RightButton : e->button(), e->state()));
}

IRKick::IRKick(const QCString &obj) : QObject(), DCOPObject(obj), npApp(QString::null)
{
    kapp->dcopClient()->setDefaultObject(obj);
	theClient = new KLircClient();

	theTrayIcon = new IRKTrayIcon();
	if(theClient->isConnected())
	{	theTrayIcon->setPixmap(SmallIcon("irkick"));
		QToolTip::add(theTrayIcon, i18n("KDE Lirc Server: Ready."));
	}
	else
	{	theTrayIcon->setPixmap(SmallIcon("irkickoff"));
		QToolTip::add(theTrayIcon, i18n("KDE Lirc Server: No infra-red remote controls found."));
		QTimer::singleShot(10000, this, SLOT(checkLirc()));
	}
	theFlashOff = new QTimer(theTrayIcon);
	connect(theFlashOff, SIGNAL(timeout()), SLOT(flashOff()));

	theResetCount = 0;
	slotReloadConfiguration();
	connect(theClient, SIGNAL(connectionClosed()), this, SLOT(slotClosed()));
	connect(theClient, SIGNAL(remotesRead()), this, SLOT(resetModes()));
	connect(theClient, SIGNAL(commandReceived(const QString &, const QString &, int)), this, SLOT(gotMessage(const QString &, const QString &, int)));

	theTrayIcon->contextMenu()->changeTitle(0, "IRKick");
	theTrayIcon->contextMenu()->insertItem(SmallIcon( "configure" ), i18n("&Configure..."), this, SLOT(slotConfigure()));
	theTrayIcon->contextMenu()->insertSeparator();
	theTrayIcon->contextMenu()->insertItem(SmallIcon( "help" ), KStdGuiItem::help().text(), (new KHelpMenu(theTrayIcon, KGlobal::instance()->aboutData()))->menu());
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
	theTrayIcon->setPixmap(SmallIcon("irkickoff"));
	KPassivePopup::message("IRKick", i18n("The infrared system has severed its connection. Remote controls are no longer available."), SmallIcon("irkick"), theTrayIcon);
	QTimer::singleShot(1000, this, SLOT(checkLirc()));
}

void IRKick::checkLirc()
{
	if(!theClient->isConnected())
		if(theClient->connectToLirc())
		{	KPassivePopup::message("IRKick", i18n("A connection to the infrared system has been made. Remote controls may now be available."), SmallIcon("irkick"), theTrayIcon);
			theTrayIcon->setPixmap(SmallIcon("irkick"));
		}
		else
			QTimer::singleShot(10000, this, SLOT(checkLirc()));
}

void IRKick::flashOff()
{
	theTrayIcon->setPixmap(SmallIcon("irkick"));
}

void IRKick::doQuit()
{
	KSimpleConfig theConfig("irkickrc");
	theConfig.setGroup("General");
	switch(KMessageBox::questionYesNoCancel(0, i18n("Should the Infrared Remote Control server start automatically when you begin KDE?"), i18n("Automatically Start?"), i18n("Start Automatically"), i18n("Do Not Start")))
	{	case KMessageBox::No: theConfig.writeEntry("AutoStart", false); break;
		case KMessageBox::Yes: theConfig.writeEntry("AutoStart", true); break;
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
	KSimpleConfig theConfig("irkickrc");
	allActions.loadFromConfig(theConfig);
	allModes.loadFromConfig(theConfig);
	if(currentModes.count() && theResetCount)
		resetModes();
}

void IRKick::slotConfigure()
{
	KApplication::startServiceByDesktopName("kcmlirc");
}

void IRKick::updateModeIcons()
{
	for(QMap<QString,QString>::iterator i = currentModes.begin(); i != currentModes.end(); ++i)
	{	Mode mode = allModes.getMode(i.key(), i.data());
		if(mode.iconFile().isNull() || mode.iconFile().isEmpty())
		{	if(currentModeIcons[i.key()])
			{	delete currentModeIcons[i.key()];
				currentModeIcons[i.key()] = 0;
			}
		}
		else
		{	if(!currentModeIcons[i.key()])
			{	currentModeIcons[i.key()] = new IRKTrayIcon();
				currentModeIcons[i.key()]->show();
				currentModeIcons[i.key()]->contextMenu()->changeTitle(0, mode.remoteName());
				currentModeIcons[i.key()]->actionCollection()->action("file_quit")->setEnabled(false);
			}
			currentModeIcons[i.key()]->setPixmap(KIconLoader().loadIcon(mode.iconFile(), KIcon::Panel));
			QToolTip::add(currentModeIcons[i.key()], mode.remoteName() + ": <b>" + mode.name() + "</b>");
		}
	}
}

bool IRKick::getPrograms(const IRAction &action, QStringList &programs)
{
	DCOPClient *theDC = KApplication::dcopClient();
	programs.clear();

	if(action.unique())
	{	if(theDC->isApplicationRegistered(action.program().utf8()))
			programs += action.program();
	}
	else
	{
		QRegExp r = QRegExp("^" + action.program() + "-(\\d+)$");
		// find all instances...
		QCStringList buf = theDC->registeredApplications();
		for(QCStringList::iterator i = buf.begin(); i != buf.end(); ++i)
		{
			QString program = QString::fromUtf8(*i);
			if(program.contains(r))
				programs += program;
		}
		if(programs.size() > 1 && action.ifMulti() == IM_DONTSEND)
			return false;
		else if(programs.size() > 1 && action.ifMulti() == IM_SENDTOTOP)
		{	QValueList<WId> s = KWinModule().stackingOrder();
			// go through all the (ordered) window pids
			for(QValueList<WId>::iterator i = s.fromLast(); i != s.end(); i--)
			{	int p = KWin::info(*i).pid;
				QString id = action.program() + "-" + QString().setNum(p);
				if(programs.contains(id))
				{	programs.clear();
					programs += id;
					break;
				}
			}
			while(programs.size() > 1) programs.remove(programs.begin());
		}
		else if(programs.size() > 1 && action.ifMulti() == IM_SENDTOBOTTOM)
		{	QValueList<WId> s = KWinModule().stackingOrder();
			// go through all the (ordered) window pids
			for(QValueList<WId>::iterator i = s.begin(); i != s.end(); ++i)
			{	int p = KWin::info(*i).pid;
				QString id = action.program() + "-" + QString().setNum(p);
				if(programs.contains(id))
				{	programs.clear();
					programs += id;
					break;
				}
			}
			while(programs.size() > 1) programs.remove(programs.begin());
		}
	}
	return true;
}

void IRKick::executeAction(const IRAction &action)
{
	DCOPClient *theDC = KApplication::dcopClient();
	QStringList programs;

	if(!getPrograms(action, programs)) return;

	// if programs.size()==0 here, then the app is definately not running.
	if(action.autoStart() && !programs.size())
	{	QString sname = ProfileServer::profileServer()->getServiceName(action.program());
		if(!sname.isNull())
		{
			KPassivePopup::message("IRKick", i18n("Starting <b>%1</b>...").arg(action.application()), SmallIcon("irkick"), theTrayIcon);
			KApplication::startServiceByDesktopName(sname);
		}
	}
	if(action.isJustStart()) return;

	if(!getPrograms(action, programs)) return;

	for(QStringList::iterator i = programs.begin(); i != programs.end(); ++i)
	{	const QString &program = *i;
		if(theDC->isApplicationRegistered(program.utf8()))
		{	QByteArray data; QDataStream arg(data, IO_WriteOnly);
			kdDebug() << "Sending data (" << program << ", " << action.object() << ", " << action.method().prototypeNR() << endl;
			for(Arguments::const_iterator j = action.arguments().begin(); j != action.arguments().end(); ++j)
			{	kdDebug() << "Got argument..." << endl;
				switch((*j).type())
				{	case QVariant::Int: arg << (*j).toInt(); break;
					case QVariant::CString: arg << (*j).toCString(); break;
					case QVariant::StringList: arg << (*j).toStringList(); break;
					case QVariant::UInt: arg << (*j).toUInt(); break;
					case QVariant::Bool: arg << (*j).toBool(); break;
					case QVariant::Double: arg << (*j).toDouble(); break;
					default: arg << (*j).toString(); break;
				}
			}
			theDC->send(program.utf8(), action.object().utf8(), action.method().prototypeNR().utf8(), data);
		}
	}
}

void IRKick::gotMessage(const QString &theRemote, const QString &theButton, int theRepeatCounter)
{
	kdDebug() << "Got message: " << theRemote << ": " << theButton << " (" << theRepeatCounter << ")" << endl;
	theTrayIcon->setPixmap(SmallIcon("irkickflash"));
	theFlashOff->start(200, true);
	if(!npApp.isNull())
	{
		QString theApp = npApp;
		npApp = QString::null;
		// send notifier by DCOP to npApp/npModule/npMethod(theRemote, theButton);
		QByteArray data; QDataStream arg(data, IO_WriteOnly);
		arg << theRemote << theButton;
		KApplication::dcopClient()->send(theApp.utf8(), npModule.utf8(), npMethod.utf8(), data);
	}
	else
	{
		if(currentModes[theRemote].isNull()) currentModes[theRemote] = "";
		IRAItList l = allActions.findByModeButton(Mode(theRemote, currentModes[theRemote]), theButton);
		if(!currentModes[theRemote].isEmpty())
			l += allActions.findByModeButton(Mode(theRemote, ""), theButton);
		bool doBefore = true, doAfter = false;
		for(IRAItList::const_iterator i = l.begin(); i != l.end(); ++i)
			if((**i).isModeChange() && !theRepeatCounter)
			{	// mode switch
				currentModes[theRemote] = (**i).modeChange();
				Mode mode = allModes.getMode(theRemote, (**i).modeChange());
				updateModeIcons();
				doBefore = (**i).doBefore();
				doAfter = (**i).doAfter();
				break;
			}

		for(int after = 0; after < 2; after++)
		{	if(doBefore && !after || doAfter && after)
				for(IRAItList::const_iterator i = l.begin(); i != l.end(); ++i)
					if(!(**i).isModeChange() && ((**i).repeat() || !theRepeatCounter))
					{	executeAction(**i);
					}
			if(!after && doAfter)
			{	l = allActions.findByModeButton(Mode(theRemote, currentModes[theRemote]), theButton);
				if(!currentModes[theRemote].isEmpty())
					l += allActions.findByModeButton(Mode(theRemote, ""), theButton);
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
	npApp = QString::null;
}

#include "irkick.moc"
