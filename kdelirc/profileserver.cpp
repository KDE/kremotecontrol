//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qfile.h>
#include <qxml.h>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include "profileserver.h"

ProfileServer *ProfileServer::theInstance = 0;

ProfileServer::ProfileServer()
{
	theProfiles.setAutoDelete(true);
	loadProfiles();
}

ProfileServer::~ProfileServer()
{
}

void ProfileServer::loadProfiles()
{
	QStringList theFiles = KGlobal::dirs()->findAllResources("data", "profiles/*.profile.xml");
	for(QStringList::iterator i = theFiles.begin(); i != theFiles.end(); i++)
	{	kdDebug() << "Found data file: " << *i << endl;
		Profile *p = new Profile();
		p->loadFromFile(*i);
		theProfiles.insert(p->id(), p);
	}
}

Profile::Profile()
{
	theActions.setAutoDelete(true);
}

void Profile::loadFromFile(const QString &fileName)
{
	charBuffer = "";
	curPA = 0;
	curPAA = 0;

	QFile xmlFile(fileName);
	QXmlInputSource source(&xmlFile);
	QXmlSimpleReader reader;
	reader.setContentHandler(this);
	reader.parse(source);
}

const ProfileAction *ProfileServer::getAction(const QString &appId, const QString &actionId) const
{
	if(theProfiles[appId])
		if(theProfiles[appId]->theActions[actionId])
			return theProfiles[appId]->theActions[actionId];
	return 0;
}

const ProfileAction *ProfileServer::getAction(const QString &appId, const QString &objId, const QString &prototype) const
{
	return getAction(appId, objId + "::" + prototype);
}

bool Profile::characters(const QString &data)
{
	charBuffer += data;
	return true;
}

bool Profile::startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes)
{
	if(name == "profile")
		theId = attributes.value("id");
	else if(name == "action")
	{
		curPA = new ProfileAction;
		curPA->setObjId(attributes.value("objid"));
		curPA->setPrototype(attributes.value("prototype"));
	}
	else if(name == "arguments")
		for(int i = 0; i < attributes.value("count").toInt(); i++)
		{	curPA->theArguments.append(ProfileActionArgument());
			curPA->theArguments.last().setAction(curPA);
		}
	else if(name == "argument")
		curPAA = &(curPA->theArguments[attributes.value("place").toInt()]);
	else if(name == "range" && curPAA)
		curPAA->setRange(qMakePair(attributes.value("min").toInt(), attributes.value("max").toInt()));

	charBuffer = "";
	return true;
}

bool Profile::endElement(const QString &, const QString &, const QString &name)
{
	if(name == "name")
		if(curPA)
			curPA->setName(charBuffer);
		else
			theName = charBuffer;
	else if(name == "author")
		theAuthor = charBuffer;
	else if(name == "type" && curPAA)
		curPAA->setType(charBuffer);
	else if(name == "comment" && curPA && !curPAA)
		curPA->setComment(charBuffer);
	else if(name == "comment" && curPA && curPAA)
		curPAA->setComment(charBuffer);
	else if(name == "action")
	{
		curPA->setProfile(this);
		theActions.insert(curPA->objId() + "::" + curPA->prototype(), curPA);
		curPA = 0;
	}
	else if(name == "argument")
		curPAA = 0;

	charBuffer = "";
	return true;
}
