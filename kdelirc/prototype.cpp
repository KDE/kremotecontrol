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
#include <qregexp.h>

#include "prototype.h"

Prototype::Prototype()
{
	original = "";
}

Prototype::Prototype(const QString &source)
{
	original = source;
	parse();
}

Prototype::~Prototype()
{
}

const QString Prototype::argumentList() const
{
	QString ret = "";
	for(unsigned i = 0; i < theTypes.count(); i++)
		ret += (i ? ", " : "") + theTypes[i] + " " + theNames[i];
	return ret;
}

const QString Prototype::argumentListNN() const
{
	QString ret = "";
	for(unsigned i = 0; i < theTypes.count(); i++)
		ret += (i ? ", " : "") + theTypes[i];
	return ret;
}

void Prototype::parse()
{
	theNames.clear();
	theTypes.clear();

	QRegExp main("^(.*) (\\w[\\d\\w]*)\\((.*)\\)");
	QRegExp parameters("^\\s*([^,\\s]+)(\\s+(\\w[\\d\\w]*))?(,(.*))?$");

	if(main.search(original) == -1) return;
	theReturn = main.cap(1);
	theName = main.cap(2);

	QString args = main.cap(3);
	while(parameters.search(args) != -1)
	{	theTypes += parameters.cap(1);
		theNames += parameters.cap(3);
		args = parameters.cap(5);
	}
}

