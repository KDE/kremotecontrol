/***************************************************************************
                          irkick.h  -  Interface of the main window
    copyright            : (C) 2002 by Gav Wood
    email                : gav@indigoarchive.net
    significant portions : (C) 2002 by Malte Starostic

***************************************************************************/

// This program is free software.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef __IRKICK_H
#define __IRKICK_H

#include <qstringlist.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qpair.h>
#include <qvariant.h>
#include <qregexp.h>

#include <ksystemtray.h>

#include <dcopobject.h>

class QSocket;
class QSocketNotifier;

class KLircClient: public QObject
{
	Q_OBJECT

private:
	struct lirc_config *theConfig;
	QSocket *theSocket;
	QSocketNotifier *theNotifier;
	QMap<QString, QStringList> theRemotes;
	bool listIsUpToDate;

	void updateRemotes();
	void sendCommand(const QString &command);
	const QString readLine();

private slots:
	void slotRead();

signals:
	/**
	 * Emitted when the list of controls / buttons was completely read
	 */
	void remotesRead();

	/**
	 * Emitted when a IR command was received
	 *
	 * The arguments are the name of the remote control used,
	 * the name of the button pressed and the repeat counter.
	 *
	 * The signal is emitted repeatedly as long as the button
	 * on the remote control remains pressed.
	 * The repeat counter starts with 0 and increases
	 * every time this signal is emitted.
	 */
	void commandReceived(const QString &remote, const QString &button, int repeatCounter);

public:
	/**
	 * Query status of connection.
	 *
	 * @returns true if connected to lircd.
	 */
	bool isConnected() const;

	/**
	 * Query status of remote list.
	 *
	 * Make sure this is true before calling remotes()/buttons(...).
	 *
	 * @returns true if up to date.
	 */
	bool haveFullList() const;

	/**
	 * Retrieve list of remote controls.
	 *
	 * @returns said list.
	 */
	const QStringList remotes() const;

	/**
	 * Retrieve list of buttons of a praticular remote control.
	 *
	 * @returns said list.
	 */
	const QStringList buttons(const QString &theRemote) const;

	KLircClient(QWidget *parent = 0, const char *name = 0);
	~KLircClient();
};

class IRAction
{
	QValueList< QPair<QString, QString> > extractParameters() const;

public:
	QString Program, Object, Method, Remote, Button;
	QValueList<QVariant> Arguments;
	bool Repeat;

public:
	const QString ArgumentString() const;
	const QString MethodMinusReturn() const;

	IRAction(const QString &theProgram, const QString &theObject, const QString &theMethod, const QValueList<QVariant> &theArguments, const QString &theRemote, const QString &theButton, bool theRepeat);
	IRAction() { Program = QString::null; };
};

class IRActions: public QMap<QPair<QString, QString>, QValueList<IRAction> >
{
};

class IRKick: public KSystemTray, virtual public DCOPObject
{
	Q_OBJECT
	K_DCOP

	QString npApp, npModule, npMethod;

	IRActions allActions;

signals:

protected:
	KLircClient *theClient;

private slots:
	void gotMessage(const QString &theRemote, const QString &theButton, int theRepeatCounter);

	void slotReloadConfiguration();
	void slotShowAbout();
	void slotShowAboutKDE();

//MOC_SKIP_BEGIN
k_dcop:
//MOC_SKIP_END
	/**
	 * Query status of connection.
	 *
	 * @returns true if connected to lircd.
	 */
	virtual bool isConnected() { return theClient->isConnected(); }

	/**
	 * Query status of remote list.
	 *
	 * Make sure this is true before calling remotes()/buttons(...).
	 *
	 * @returns true if up to date.
	 */
	virtual bool haveFullList() { return theClient->haveFullList(); }

	/**
	 * Retrieve list of remote controls.
	 *
	 * @returns said list.
	 */
	virtual const QStringList remotes() { return theClient->remotes(); }

	/**
	 * Retrieve list of buttons of a praticular remote control.
	 *
	 * @returns said list.
	 */
	virtual const QStringList buttons(QString theRemote) { return theClient->buttons(theRemote); }

	/**
	 * Sends next keypress to given application by DCOP.
	 *
	 * @param The receiving application.
	 * @param The receiving application module.
	 * @param The method name. Must have two QString parameters.
	 */
	virtual void stealNextPress(QString app, QString module, QString method);

	/**
	 * Reloads configuration from file(s)
	 */
	virtual void reloadConfiguration() { slotReloadConfiguration(); }

public:
	IRKick(QWidget *parent = 0, const char *name = 0);
	~IRKick();
};

#endif
