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

#include "main.h"
#include "irkick.h"

IRKick::IRKick(QWidget *parent, const char *name) : KSystemTray(parent, name), DCOPObject("IRKick"), npApp(QString::null)
{
	setPixmap(SmallIcon("irkick"));
	QToolTip::add(this, "Ready.");
	theClient = new KLircClient(this);
	connect(theClient, SIGNAL(commandReceived(const QString &, const QString &, int)), this, SLOT(gotMessage(const QString &, const QString &, int)));
	contextMenu()->insertItem("Reload", this, SLOT(slotReloadConfiguration()));
	contextMenu()->insertItem("About IRKick...", this, SLOT(slotShowAbout()));
	contextMenu()->insertItem("About KDE...", this, SLOT(slotShowAboutKDE()));
	slotReloadConfiguration();
}

IRKick::~IRKick()
{
}

void IRKick::slotReloadConfiguration()
{
	// clear configuration
	allActions.clear();

	// load configuration from config file
	KSimpleConfig theConfig("irkickrc");
	int numBindings = theConfig.readNumEntry("Bindings");
	for(int i = 0; i < numBindings; i++)
	{	QString Binding = "Binding" + QString().setNum(i);

		int numArguments = theConfig.readNumEntry(Binding + "Arguments");
		QValueList<QVariant> Arguments;
		for(int j = 0; j < numArguments; j++)
		{	QVariant::Type theType = (QVariant::Type)theConfig.readNumEntry(Binding + "ArgumentType" + QString().setNum(j), QVariant::String);
			Arguments += theConfig.readPropertyEntry(Binding + "Argument" + QString().setNum(j), theType == QVariant::CString ? QVariant::String : theType);
			Arguments.last().cast(theType);
		}

		allActions[qMakePair(theConfig.readEntry(Binding + "Remote"), theConfig.readEntry(Binding + "Button"))] +=
					IRAction( theConfig.readEntry(Binding + "Program"), theConfig.readEntry(Binding + "Object"),
					theConfig.readEntry(Binding + "Method"), Arguments, theConfig.readEntry(Binding + "Remote"),
					theConfig.readEntry(Binding + "Button"), theConfig.readBoolEntry(Binding + "Repeat"));
	}
}

void IRKick::gotMessage(const QString &theRemote, const QString &theButton, int theRepeatCounter)
{
	if(npApp != QString::null)
	{
		// send notifier by DCOP to npApp/npModule/npMethod(theRemote, theButton);
		if(!DCOPRef(QCString(npApp), QCString(npModule)).call(QCString(npMethod), theRemote, theButton).isValid())
			KPassivePopup::message("IRKick", "Error: Couldn't contact application " + npApp + " to send keypress.", SmallIcon("package_applications"), this);
		npApp = QString::null;
	}
	else
	{
		const QValueList<IRAction> &l = allActions[qMakePair(theRemote, theButton)];
		if(!l.isEmpty())
			for(QValueList<IRAction>::const_iterator i = l.begin(); i != l.end(); i++)
				if((*i).Repeat || !theRepeatCounter)
				{	DCOPClient *theDC = KApplication::dcopClient();
					if(theDC->isApplicationRegistered(QCString((*i).Program)))
					{	QByteArray data; QDataStream arg(data, IO_WriteOnly);
						for(QValueList<QVariant>::const_iterator j = (*i).Arguments.begin(); j != (*i).Arguments.end(); j++)
							switch((*j).type())
							{	case QVariant::Int: arg << (*j).toInt(); break;
								case QVariant::CString: arg << (*j).toCString(); break;
								case QVariant::StringList: arg << (*j).toStringList(); break;
								case QVariant::UInt: arg << (*j).toUInt(); break;
								case QVariant::Bool: arg << (*j).toBool(); break;
								case QVariant::Double: arg << (*j).toDouble(); break;
								default: arg << (*j).toString(); break;
							}
						theDC->send(QCString((*i).Program), QCString((*i).Object), QCString((*i).MethodMinusReturn()), data);
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

void IRKick::slotShowAboutKDE()
{
	KAboutKDE *about = new KAboutKDE(this);
	about->exec();
	delete about;
}

void IRKick::slotShowAbout()
{
	KAboutDialog *about = new KAboutDialog(KAboutDialog::AbtAppStandard, "IRKick", KDialogBase::Close, KDialogBase::Close, this, "name", true);
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
									"then feel free to send me a mail.\n"), this));
		c->addPerson(QString::null, QString("gav@kde.org"), QString("http://irkick.sourceforge.net/"), QString::null, true);
	}
	c = about->addContainerPage(i18n("&Credits"), AlignCenter, AlignLeft);
	if( c != 0 )
		c->addWidget(new QLabel(i18n("<p align=left>Author:<br/><b>Gav Wood</b></p>"), this));
	about->addLicensePage("&License Agreement", aboutData->license());
	about->exec();
	delete about;
}

