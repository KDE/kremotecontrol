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
		Profile p(*i);
		theProfiles[p.id()] = p;
	}
}

Profile::Profile()
{
}

Profile::Profile(const QString &fileName)
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

bool Profile::characters(const QString &data)
{
	charBuffer += data;
	return true;
}

bool Profile::startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes)
{
	if(name == "profile")
	{	theId = attributes.value("id");
	}
	else if(name == "action")
	{
		curPA = new ProfileAction;
		curPA->setAppId(attributes.value("appid"));
		curPA->setObjId(attributes.value("objid"));
		curPA->setPrototype(attributes.value("prototype"));
	}

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
	else if(name == "comment" && curPA)
		curPA->setComment(charBuffer);
	else if(name == "action")
	{
		theActions[curPA->name()] = *curPA;
		delete curPA;
		curPA = 0;
	}

	charBuffer = "";
	return true;
}
