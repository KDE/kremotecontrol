/***************************************************************************
                          irkick.cpp  -  Implementation of the main window
                             -------------------
    copyright            : (C) 2002 by Gav Wood
    email                : gav@indigoarchive.net
***************************************************************************/

// This program is free software.

#include <qwidget.h>
#include <qdialog.h>
#include <qtooltip.h>

#include <kapplication.h>
#include <kaction.h>
#if KDE_IS_VERSION(3, 1, 90)
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

#include <dcopclient.h>
#include <dcopref.h>

#include "profileserver.h"
#include "irkick.h"

extern "C"
{
	KDEDModule *create_irkick(const QCString &name)
	{
		return new IRKick(name);
	}
}

IRKick::IRKick(const QCString &obj) : KDEDModule(obj), npApp(QString::null)
{
	theClient = new KLircClient();
	theTrayIcon = new KSystemTray();
	theTrayIcon->setPixmap(SmallIcon("irkick"));
//	QToolTip::add(this, "Ready.");
	connect(theClient, SIGNAL(commandReceived(const QString &, const QString &, int)), this, SLOT(gotMessage(const QString &, const QString &, int)));
#if KDE_IS_VERSION(3, 1, 90)
	theTrayIcon->contextMenu()->changeTitle(0, "IRKick");
	theTrayIcon->contextMenu()->insertItem("Reload", this, SLOT(slotReloadConfiguration()));
	theTrayIcon->contextMenu()->insertItem("About IRKick...", this, SLOT(slotShowAbout()));
	theTrayIcon->contextMenu()->insertItem("About KDE...", this, SLOT(slotShowAboutKDE()));
	theTrayIcon->actionCollection()->action("file_quit")->setEnabled(false);
#endif
	slotReloadConfiguration();
	aboutData = new KAboutData("irkick", I18N_NOOP("IRKick"), VERSION, I18N_NOOP("IRKick"), KAboutData::License_GPL, "(c) 2003, Gav Wood", 0, 0, "gav@kde.org");
	aboutData->addAuthor("Gav Wood", 0, "gav@kde.org");
	theTrayIcon->show();
}

IRKick::~IRKick()
{
	delete theTrayIcon;
}

void IRKick::slotReloadConfiguration()
{
	// load configuration from config file
	KSimpleConfig theConfig("irkickrc");
	allActions.loadFromConfig(theConfig);
	if(currentModes.count())
	{	currentModes.clear();
		KPassivePopup::message("IRKick", "Resetting all modes.", SmallIcon("package_applications"), theTrayIcon);
	}
}

void IRKick::gotMessage(const QString &theRemote, const QString &theButton, int theRepeatCounter)
{
	if(npApp != QString::null)
	{
		QString theApp = npApp;
		npApp = QString::null;
		// send notifier by DCOP to npApp/npModule/npMethod(theRemote, theButton);
/*		QByteArray data; QDataStream arg(data, IO_WriteOnly);
		arg << theRemote << theButton;
		QCString rType; QByteArray rData;
		if(!KApplication::dcopClient()->call(QCString(npApp), QCString(npModule), QCString(npMethod), data, rType, rData))
		{	kdDebug() << "ERROR!!!" << endl;
			// BUT WHY?!?!?!
		}
*/
// this code works, but i want to figure out why the code above which should be equivalent doesn't work
		if(!DCOPRef(QCString(theApp), QCString(npModule)).call(QCString(npMethod), theRemote, theButton).isValid())
			KPassivePopup::message("IRKick", "Error: Couldn't contact application " + theApp + " to send keypress.", SmallIcon("package_applications"), theTrayIcon);
	}
	else
	{
		if(currentModes[theRemote].isNull()) currentModes[theRemote] = "";
		IRAItList l = allActions.findByModeButton(Mode(theRemote, currentModes[theRemote]), theButton);
		if(currentModes[theRemote] != "")
			l += allActions.findByModeButton(Mode(theRemote, ""), theButton);
		if(!l.isEmpty())
			for(IRAItList::const_iterator i = l.begin(); i != l.end(); i++)
				if((**i).program() == "")
				{	// mode switch
					currentModes[theRemote] = (**i).object();
					if((**i).object() != "")
						KPassivePopup::message("IRKick", "Switching mode on <b>" + theRemote + "</b> to <b>" + (**i).object() + "</b>.", SmallIcon("package_applications"), theTrayIcon);
					else
						KPassivePopup::message("IRKick", "Exiting mode on <b>" + theRemote + "</b>.", SmallIcon("package_applications"), theTrayIcon);
				}
				else
					if((**i).repeat() || !theRepeatCounter)
					{	DCOPClient *theDC = KApplication::dcopClient();
						if((**i).autoStart() && !theDC->isApplicationRegistered(QCString((**i).program())))
						{	// start it up, and wait for it to become available
							QString sname = ProfileServer::profileServer()->getServiceName((**i).program());
							if(sname != QString::null)
							{
								KPassivePopup::message("IRKick", "Starting <b>" + (**i).application() + "</b>...", SmallIcon("package_applications"), theTrayIcon);
								KApplication::startServiceByName(sname);
							}
						}

						if(theDC->isApplicationRegistered(QCString((**i).program())))
						{	QByteArray data; QDataStream arg(data, IO_WriteOnly);
							kdDebug() << "Sending data (" << QCString((**i).program()) << ", " << QCString((**i).object()) << ", " << QCString((**i).method().prototypeNR()) << endl;
							for(Arguments::const_iterator j = (**i).arguments().begin(); j != (**i).arguments().end(); j++)
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
							theDC->send(QCString((**i).program()), QCString((**i).object()), QCString((**i).method().prototypeNR()), data);
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

void IRKick::slotShowAboutKDE()
{
	KAboutKDE *about = new KAboutKDE(theTrayIcon);
	about->exec();
	delete about;
}

void IRKick::slotShowAbout()
{
	KAboutDialog *about = new KAboutDialog(KAboutDialog::AbtAppStandard, "IRKick", KDialogBase::Close, KDialogBase::Close, theTrayIcon, "name", true);
	about->setTitle(i18n("KDE Lirc Server"));
	about->setProduct(i18n("IRKick"), VERSION, i18n("Gav Wood"), "2003");
	KAboutContainer *c = about->addContainerPage(i18n("&About"), AlignCenter, AlignLeft);
	if( c != 0 )
	{	c->addWidget(new QLabel(i18n("IRKick: The KDE Linux Infrared Remote Control Server.\n\n"
									"IRKick is made to be as simple, intuitive and user-centric\n"
									"as possible. If you\n\n"
									"1. have a suggestion for improvement\n"
									"2. have found a bug\n"
									"3. want to contribute with something\n\n"
									"then feel free to send me a mail.\n"), theTrayIcon));
		c->addPerson(QString::null, QString("gav@kde.org"), QString("http://www.indigoarchive.net/gav/"), QString::null, true);
	}
	c = about->addContainerPage(i18n("&Credits"), AlignCenter, AlignLeft);
	if( c != 0 )
		c->addWidget(new QLabel(i18n("<p align=left>Author:<br/><b>Gav Wood</b></p>"), theTrayIcon));
	about->addLicensePage("&License Agreement", aboutData->license());
	about->exec();
	delete about;
}

