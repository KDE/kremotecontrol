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
#ifndef PROFILESERVER_H
#define PROFILESERVER_H

#include <qpair.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qxml.h>

/**
@author Gav Wood
*/

typedef QPair<int,int> Range;

class ProfileActionArgument
{
	QString theComment, theType;
	Range theRange;

public:
	const QString &comment() const { return theComment; }
	void setComment(const QString &a) { theComment = a; }
	const QString &type() const { return theType; }
	void setType(const QString &a) { theType = a; }
	const Range &range() const { return theRange; }
	void setRange(const Range &a) { theRange = a; }
};

class ProfileAction
{
	QString theAppId, theObjId, thePrototype, theName, theComment;
public:
	QValueList<ProfileActionArgument> theArguments;

public:
	const QString &appId() const { return theAppId; }
	void setAppId(const QString &a) { theAppId = a; }
	const QString &objId() const { return theObjId; }
	void setObjId(const QString &a) { theObjId = a; }
	const QString &prototype() const { return thePrototype; }
	void setPrototype(const QString &a) { thePrototype = a; }
	const QString &name() const { return theName; }
	void setName(const QString &a) { theName = a; }
	const QString &comment() const { return theComment; }
	void setComment(const QString &a) { theComment = a; }
	const QValueList<ProfileActionArgument> &arguments() { return theArguments; }
};

class Profile : public QXmlDefaultHandler
{
	QString theId, theName, theAuthor;
	QString charBuffer;

	ProfileAction *curPA;
	ProfileActionArgument *curPAA;
public:
	QMap<QString, ProfileAction> theActions;

public:
	bool characters(const QString &data);
	bool startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes);
	bool endElement(const QString &, const QString &, const QString &name);

public:
	QString id() const { return theId; }
	void setId(const QString &a) { theId = a; }
	QString name() const { return theName; }
	void setName(const QString &a) { theName = a; }
	const QMap<QString, ProfileAction> &actions() const { return theActions; }

	Profile(const QString &fileName);
	Profile();
};

class ProfileServer
{
	static ProfileServer *theInstance;
	void loadProfiles();
	QMap<QString, Profile> theProfiles;

public:
	static ProfileServer *profileServer() { if(!theInstance) theInstance = new ProfileServer(); return theInstance; }

	const QMap<QString, Profile> profiles() const { return theProfiles; }

	ProfileServer();
	~ProfileServer();
};

#endif
