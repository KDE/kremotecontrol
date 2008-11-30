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

#include <QMap>
#include <qxml.h>
#include <QHash>

/**
@author Gav Wood
*/

enum IfMulti {IM_DONTSEND, IM_SENDTOALL, IM_SENDTOTOP, IM_SENDTOBOTTOM};

typedef QPair<int,int> Range;

class ProfileAction;
class Profile;

class ProfileActionArgument
{
	QString theComment, theType;
	Range theRange;
	QString theDefault;		// should be QVariant?
	const ProfileAction *parent;

	friend class Profile;
public:
	const QString &comment() const { return theComment; }
	void setComment(const QString &a) { theComment = a; }
	const QString &type() const { return theType; }
	void setType(const QString &a) { theType = a; }
	const QString &getDefault() const { return theDefault; }
	void setDefault(const QString &a) { theDefault = a; }
	const Range &range() const { return theRange; }
	void setRange(const Range &a) { theRange = a; }

	const ProfileAction *action() const { return parent; }
	void setAction(const ProfileAction *a) { parent = a; }
};

class ProfileAction
{
	QString theObjId, thePrototype, theName, theComment, theClass;
	float theMultiplier;
	const Profile *parent;
	bool theRepeat, theAutoStart;
	QList<ProfileActionArgument> theArguments;

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
	const QString &getClass() const { return theClass; }
	void setClass(const QString &a) { theClass = a; }
	float multiplier() const { return theMultiplier; }
	void setMultiplier(const float a) { theMultiplier = a; }
	bool repeat() const { return theRepeat; }
	void setRepeat(bool a) { theRepeat = a; }
	bool autoStart() const { return theAutoStart; }
	void setAutoStart(bool a) { theAutoStart = a; }
	const QList<ProfileActionArgument> &arguments() const { return theArguments; }

	const Profile *profile() const { return parent; }
	void setProfile(const Profile *a) { parent = a; }
};

class Profile : public QXmlDefaultHandler
{
	QString theId, theName, theAuthor, theServiceName;
	IfMulti theIfMulti;
	bool theUnique;
	QString charBuffer;

	ProfileAction *curPA;
	ProfileActionArgument *curPAA;
	QHash<QString, ProfileAction*> theActions;		// objid+"::"+prototype => ProfileAction

	friend class ProfileServer;
public:
	bool characters(const QString &data);
	bool startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes);
	bool endElement(const QString &, const QString &, const QString &name);

	const QString &id() const { return theId; }
	void setId(const QString &a) { theId = a; }
	const QString &name() const { return theName; }
	void setName(const QString &a) { theName = a; }
	const QString &author() const { return theAuthor; }
	void setAuthor(const QString &a) { theAuthor = a; }
	bool unique() const { return theUnique; }
	void setUnique(const bool a) { theUnique = a; }
	IfMulti ifMulti() const { return theIfMulti; }
	void setIfMulti(const IfMulti a) { theIfMulti = a; }
	const QString &serviceName() const { if(theServiceName != QString()) return theServiceName; return theName; }
	void setServiceName(const QString &a) { theServiceName = a; }
	const QHash<QString, ProfileAction*> &actions() const { return theActions; }
	const ProfileAction *searchClass(const QString &c) const;

	void loadFromFile(const QString &fileName);

	Profile();
};

class ProfileServer
{
	static ProfileServer *theInstance;
	void loadProfiles();
	QHash<QString, Profile*> theProfiles;			// id => Profile

public:
	static ProfileServer *profileServer() { if(!theInstance) theInstance = new ProfileServer(); return theInstance; }
	const QHash<QString, Profile*> profiles() const { return theProfiles; }
	const ProfileAction *getAction(const QString &appId, const QString &objId, const QString &prototype) const;
	const ProfileAction *getAction(const QString &appId, const QString &actionId) const;
	const QString &getServiceName(const QString &appId) const;

	ProfileServer();
	~ProfileServer();
};

#endif
