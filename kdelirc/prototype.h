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
#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <qstringlist.h>
#include <qpair.h>
#include <qstring.h>

/**
@author Gav Wood
*/

class Prototype
{
	QString original, theName, theReturn;
	QStringList theNames, theTypes;

	void parse();

public:
	unsigned count() const{ return theTypes.count(); }
	const QPair<QString, QString> operator[](int i) const { return qMakePair(theTypes[i], theNames[i]); }
	const QString &name(int i) const { return theNames[i]; }
	const QString &type(int i) const { return theTypes[i]; }
	const QString &returnType() const { return theReturn; }
	const QString &name() const { return theName; }
	const QString &prototype() const { return original; }
	const QString argumentList() const;
	const QString argumentListNN() const;
	const int argumentCount() { return theTypes.count(); }
	const QString prototypeNR() const { return theName + "(" + argumentListNN() + ")"; }

	void setPrototype(const QString &source) { original = source; parse(); }

	Prototype &operator=(const QString &source) { setPrototype(source); return *this; }

	Prototype(const QString &source);
	Prototype();
	~Prototype();

};

#endif
