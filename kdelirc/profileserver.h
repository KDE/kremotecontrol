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
#include <qdict.h>

/**
@author Gav Wood
*/

typedef QPair<int,int> Range;

class ProfileAction;
class Profile;

class ProfileActionArgument
{
	QString theComment, theType;
	Range theRange;
	const ProfileAction *parent;

	friend class Profile;
public:
	const QString &comment() const { return theComment; }
	void setComment(const QString &a) { theComment = a; }
	const QString &type() const { return theType; }
	void setType(const QString &a) { theType = a; }
	const Range &range() const { return theRange; }
	void setRange(const Range &a) { theRange = a; }

	const ProfileAction *action() const { return parent; }
	void setAction(const ProfileAction *a) { parent = a; }
};

class ProfileAction
{
	QString theObjId, thePrototype, theName, theComment;
	const Profile *parent;
	QValueList<ProfileActionArgument> theArguments;

	friend class Profile;
public:
	const QString &objId() const { return theObjId; }
	void setObjId(const QString &a) { theObjId = a; }
	const QString &prototype() const { return thePrototype; }
	void setPrototype(const QString &a) { thePrototype = a; }
	const QString &name() const { return theName; }
	void setName(const QString &a) { theName = a; }
	const QString &comment() const { return theComment; }
	void setComment(const QString &a) { theComment = a; }
	const QValueList<ProfileActionArgument> &arguments() const { return theArguments; }

	const Profile *profile() const { return parent; }
	void setProfile(const Profile *a) { parent = a; }
};

class Profile : public QXmlDefaultHandler
{
	QString theId, theName, theAuthor;
	QString charBuffer;

	ProfileAction *curPA;
	ProfileActionArgument *curPAA;
	QDict<ProfileAction> theActions;		// objid+"::"+prototype => ProfileAction

	friend class ProfileServer;
public:
	bool characters(const QString &data);
	bool startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes);
	bool endElement(const QString &, const QString &, const QString &name);

	QString id() const { return theId; }
	void setId(const QString &a) { theId = a; }
	QString name() const { return theName; }
	void setName(const QString &a) { theName = a; }
	QString author() const { return theAuthor; }
	void setAuthor(const QString &a) { theAuthor = a; }
	const QDict<ProfileAction> &actions() const { return theActions; }

	void loadFromFile(const QString &fileName);

	Profile();
};

class ProfileServer
{
	static ProfileServer *theInstance;
	void loadProfiles();
	QDict<Profile> theProfiles;			// id => Profile

public:
	static ProfileServer *profileServer() { if(!theInstance) theInstance = new ProfileServer(); return theInstance; }
	const QDict<Profile> profiles() const { return theProfiles; }
	const ProfileAction *getAction(const QString &appId, const QString &objId, const QString &prototype) const;
	const ProfileAction *getAction(const QString &appId, const QString &actionId) const;

	ProfileServer();
	~ProfileServer();
};

#endif
