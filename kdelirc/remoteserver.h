//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef REMOTESERVER_H
#define REMOTESERVER_H

#include <qstring.h>
#include <qxml.h>
#include <qdict.h>

/**
@author Gav Wood
*/


class RemoteButton
{
	QString theName, theId, theClass, theParameter;

	friend class Remote;
public:
	void setName(const QString &a) { theName = a; }
	const QString &name(void) const { return theName; }
	void setClass(const QString &a) { theClass = a; }
	const QString &getClass(void) const { return theClass; }
	void setParameter(const QString &a) { theParameter = a; }
	const QString &parameter(void) const { return theParameter; }
	void setId(const QString &a) { theId = a; }
	const QString &id(void) const { return theId; }
};

class Remote : public QXmlDefaultHandler
{
	QString theName, theId, theAuthor;
	QDict<RemoteButton> theButtons;

	QString charBuffer;
	RemoteButton *curRB;

	friend class RemoteServer;
public:
	bool characters(const QString &data);
	bool startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes);
	bool endElement(const QString &, const QString &, const QString &name);

	void setName(const QString &a) { theName = a; }
	const QString &name(void) const { return theName; }
	void setId(const QString &a) { theId = a; }
	const QString &id(void) const { return theId; }
	void setAuthor(const QString &a) { theAuthor = a; }
	const QString &author(void) const { return theAuthor; }
	const QDict<RemoteButton> &buttons() const { return theButtons; }

	void loadFromFile(const QString &fileName);

	const QString &getButtonName(const QString &id) const { if(theButtons[id]) return theButtons[id]->name(); return id; }

	Remote();
	~Remote();
};

class RemoteServer
{
	static RemoteServer *theInstance;
	void loadRemotes();
	QDict<Remote> theRemotes;

public:
	static RemoteServer *remoteServer() { if(!theInstance) theInstance = new RemoteServer(); return theInstance; }

	const QDict<Remote> &remotes() const { return theRemotes; }

	const QString &getRemoteName(const QString &id) const { if(theRemotes[id]) return theRemotes[id]->name(); return id; }
	const QString &getButtonName(const QString &remote, const QString &button) const { if(theRemotes[remote]) return theRemotes[remote]->getButtonName(button); return button; }

	RemoteServer();
	~RemoteServer();
};

#endif
