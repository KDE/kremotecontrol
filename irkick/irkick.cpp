/***************************************************************************
                          irkick.cpp  -  Implementation of the main window
                             -------------------
    copyright            : (C) 2002 by Gav Wood
    email                : gav@indigoarchive.net
***************************************************************************/

// This program is free software.

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>

#include <qwidget.h>
#include <qdialog.h>
#include <qtooltip.h>
#include <qsocket.h>
#include <qsocketnotifier.h>

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

KLircClient::KLircClient(QWidget *parent, const char *name) : QObject(parent, name), theSocket(0), listIsUpToDate(false)
{
	int sock = ::socket(PF_UNIX, SOCK_STREAM, 0);
	if(sock == -1)
	{
		KMessageBox::sorry(0, i18n("Could not create a socket to receive infrared signals. The error is:\n") + strerror(errno));
		return;
	}
	sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "/dev/lircd");
	if(::connect(sock, (struct sockaddr *)(&addr), sizeof(addr)) == -1)
	{
		KMessageBox::sorry(0, i18n("Could not establish a connection to receive infrared signals. The error is:\n") + strerror(errno));
		::close(sock);
		return;
	}

	theSocket = new QSocket;
	theSocket->setSocket(sock);
	connect(theSocket, SIGNAL(readyRead()), SLOT(slotRead()));
	updateRemotes();
}

KLircClient::~KLircClient()
{
	if(theSocket)
		delete [] theSocket;
}

const QStringList KLircClient::remotes() const
{
	QStringList remotes;
	for(QMap<QString, QStringList>::ConstIterator i = theRemotes.begin(); i != theRemotes.end(); i++)
		remotes.append(i.key());
	remotes.sort();
	return remotes;
}

const QStringList KLircClient::buttons(const QString &theRemote) const
{
	return theRemotes[theRemote];
}

void KLircClient::slotRead()
{
	while (theSocket->bytesAvailable())
	{
		QString line = readLine();
		if (line == "BEGIN")
		{
			// BEGIN
			// <command>
			// [SUCCESS|ERROR]
			// [DATA
			// n
			// n lines of data]
			// END
			line = readLine();
			if (line == "SIGHUP")
			{
				// Configuration changed
				do line = readLine();
				while (!line.isEmpty() && line != "END");
				updateRemotes();
				return;
			}
			else if (line == "LIST")
			{
				// remote control list
				if (readLine() != "SUCCESS" || readLine() != "DATA")
				{
					do line = readLine();
					while (!line.isEmpty() && line != "END");
					return;
				}
				QStringList remotes;
				int count = readLine().toInt();
				for (int i = 0; i < count; ++i)
					remotes.append(readLine());
				do line = readLine();
				while (!line.isEmpty() && line != "END");
				if (line.isEmpty())
					return; // abort on corrupt data
				for (QStringList::ConstIterator it = remotes.begin(); it != remotes.end(); ++it)
					sendCommand("LIST " + *it);
				return;
			}
			else if (line.left(4) == "LIST")
			{
				// button list
				if (readLine() != "SUCCESS" || readLine() != "DATA")
				{
					do line = readLine();
					while (!line.isEmpty() && line != "END");
					return;
				}
				QString remote = line.mid(5);
				QStringList buttons;
				int count = readLine().toInt();
				for (int i = 0; i < count; ++i)
				{
					// <code> <name>
					QString btn = readLine();
					buttons.append(btn.mid(17));
				}
				theRemotes.insert(remote, buttons);
			}
			do line = readLine();
			while (!line.isEmpty() && line != "END");
			listIsUpToDate = true;
			emit remotesRead();
		}
		else
		{
			// <code> <repeat> <button name> <remote control name>
			line.remove(0, 17); // strip code
			int pos = line.find(' ');
			if (pos < 0) return;
			bool ok;
			int repeat = line.left(pos).toInt(&ok, 16);
			if (!ok) return;
			line.remove(0, pos + 1);

			pos = line.find(' ');
			if (pos < 0) return;
			QString btn = line.left(pos);
			line.remove(0, pos + 1);

			emit commandReceived(line, btn, repeat);
		}
	}
}

void KLircClient::updateRemotes()
{
	listIsUpToDate = false;
	theRemotes.clear();
	sendCommand("LIST");
}

bool KLircClient::isConnected() const
{
	return theSocket->state() == QSocket::Connected;
}

bool KLircClient::haveFullList() const
{
	return listIsUpToDate;
}

const QString KLircClient::readLine()
{
	if (!theSocket->bytesAvailable())
		return QString::null;

	QString line = theSocket->readLine();
	if (line.isEmpty())
		return QString::null;

	line.remove(line.length() - 1, 1);
	return line;
}

void KLircClient::sendCommand(const QString &command)
{
	QString cmd = command + "\n";
	theSocket->writeBlock(cmd.latin1(), cmd.length());
}

IRAction::IRAction(const QString &theProgram, const QString &theObject, const QString &theMethod, const QValueList<QVariant> &theArguments, const QString &theRemote, const QString &theButton, bool theRepeat)
{
	Program = theProgram;
	Object = theObject;
	Method = theMethod;
	Arguments = theArguments;
	Remote = theRemote;
	Button = theButton;
	Repeat = theRepeat;
}

QValueList< QPair<QString, QString> > IRAction::extractParameters() const
{
	QRegExp main("^(.*) (\\w[\\d\\w]*)\\((.*)\\)");
	QValueList< QPair<QString, QString> > ret;
	if(main.search(Method) == -1)
		return ret;
	ret += qMakePair(main.cap(1), main.cap(2));
	QRegExp parameters("^\\s*([^,\\s]+)(\\s+(\\w[\\d\\w]*))?(,(.*))?$");
	QString args = main.cap(3);
	while(parameters.search(args) != -1)
	{	ret += qMakePair(parameters.cap(1), parameters.cap(3));
		args = parameters.cap(5);
	}

	return ret;
}


const QString IRAction::ArgumentString() const
{
	QString ret = "";
	for(QValueList<QVariant>::const_iterator i = Arguments.begin(); i != Arguments.end(); i++)
	{	QString s = (*i).toString();
		if(s == QString::null) s = "...";
		if(i != Arguments.begin()) ret += ", ";
		ret += s;
	}
	return ret;
}

const QString IRAction::MethodMinusReturn() const
{
	QValueList< QPair<QString, QString> > all = extractParameters();
	QString ret = all[0].second + "(";
	for(unsigned i = 1; i < all.count(); i++)
	{	if(i > 1) ret += ",";
		ret += all[i].first;
	}
	return ret + ")";
}

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
			{	DCOPClient *theDC = KApplication::dcopClient();
				KPassivePopup::message("IRKick", "Executing: " + (*i).Program + "::" + (*i).Object + "." + (*i).Method +" (" + (*i).ArgumentString() + ")", SmallIcon("package_applications"), this);
				if(theDC->isApplicationRegistered(QCString((*i).Program)))
				{	QByteArray data; QDataStream arg(data, IO_WriteOnly);
					for(QValueList<QVariant>::const_iterator j = (*i).Arguments.begin(); j != (*i).Arguments.end(); j++) arg << (*j);
					KPassivePopup::message("IRKick", "Sending...", SmallIcon("package_applications"), this);
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
	about->setTitle(i18n("KDE Lirc Client"));
	about->setProduct(i18n("IRKick"), VERSION, i18n("Gav Wood"), "2002");
	KAboutContainer *c = about->addContainerPage(i18n("&About"), AlignCenter, AlignLeft);
	if( c != 0 )
	{	c->addWidget(new QLabel(i18n("IRKick: The KDE Linux Infrared Remote Control Server.\n\n"
									"IRKick is made to be as simple, intuitive and user-centric\n"
									"as possible. If you\n\n"
									"1. have a suggestion for improvement\n"
									"2. have found a bug\n"
									"3. want to contribute with something\n\n"
									"then feel free to send me a mail.\n"), this));
		c->addPerson(QString::null, QString("gav@indigoarchive.net"), QString("http://irkick.sourceforge.net/"), QString::null, true);
	}
	c = about->addContainerPage(i18n("&Credits"), AlignCenter, AlignLeft);
	if( c != 0 )
		c->addWidget(new QLabel(i18n("<p align=left>Author:<br/><b>Gav Wood</b></p>"), this));
	about->addLicensePage("&License Agreement", aboutData->license());
	about->exec();
	delete about;
}

