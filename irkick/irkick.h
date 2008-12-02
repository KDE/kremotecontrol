/***************************************************************************
                          irkick.h  -  Interface of the main window
    copyright            : (C) 2002 by Gav Wood
    email                : gav@kde.org

***************************************************************************/

// This program is free software.

#ifndef __IRKICK_H
#define __IRKICK_H


#include <QMouseEvent>

#include <ksystemtrayicon.h>
#include <kdedmodule.h>
#include <kaboutdata.h>
#include <kmenu.h>

#include "modes.h"
#include "iractions.h"
#include "klircclient.h"

class QMouseEvent;

class IRKTrayIcon: public KSystemTrayIcon
{

public:
	QMenu* contextMenu() const { return KSystemTrayIcon::contextMenu(); }
	KActionCollection* actionCollection() { return KSystemTrayIcon::actionCollection(); }

	explicit IRKTrayIcon(QWidget *parent = 0, const char *name = 0): KSystemTrayIcon(parent) {Q_UNUSED(name)}
};

class IRKick: public QObject
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.kde.irkick");


	QString npApp, npModule, npMethod;
	QMap<QString, QString> currentModes;
	QMap<QString, IRKTrayIcon *> currentModeIcons;
	IRActions allActions;
	int theResetCount;
	Modes allModes;

	IRKTrayIcon *theTrayIcon;
	KAboutData *aboutData;
	QTimer *theFlashOff;

	void updateModeIcons();

protected:
	KLircClient *theClient;

public Q_SLOTS: //dbus slot
	/**
	 * Query status of connection.
	 *
	 * @returns true if connected to lircd.
	 */
	bool isConnected() { return theClient->isConnected(); }

	/**
	 * Query status of remote list.
	 *
	 * Make sure this is true before calling remotes()/buttons(...).
	 *
	 * @returns true if up to date.
	 */
	bool haveFullList() { return theClient->haveFullList(); }

	/**
	 * Retrieve list of remote controls.
	 *
	 * @returns said list.
	 */
	const QStringList remotes() { return theClient->remotes(); }

	/**
	 * Retrieve list of buttons of a praticular remote control.
	 *
	 * @returns said list.
	 */
	const QStringList buttons(QString theRemote) { return theClient->buttons(theRemote); }

	/**
	 * Sends next keypress to given application by DCOP.
	 *
	 * @param The receiving application.
	 * @param The receiving application module.
	 * @param The method name. Must have two QString parameters.
	 */
	void stealNextPress(QString app, QString module, QString method);

	/**
	 * Cancels the proceedings of the previous stealNextPress call, if any.
	 */
	void dontStealNextPress();

	/**
	 * Reloads configuration from file(s)
	 */
	void reloadConfiguration() { slotReloadConfiguration(); }

private slots:
	void gotMessage(const QString &theRemote, const QString &theButton, int theRepeatCounter);
	void resetModes();
	void doQuit();
	void flashOff();
	void checkLirc();

	void slotConfigure();
	void slotReloadConfiguration();
	void slotClosed();
private:
	void executeAction(const IRAction &action);
	bool getPrograms(const IRAction &action, QStringList &populous);

public:
	explicit IRKick(const QString &obj);
	virtual ~IRKick();
};

#endif
