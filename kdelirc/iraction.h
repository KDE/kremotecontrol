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
#ifndef IRACTION_H
#define IRACTION_H

#include <qstring.h>

#include "prototype.h"
#include "arguments.h"

/**
@author Gav Wood
*/

class KConfig;

class IRAction
{
private:
	QString theProgram, theObject, theRemote, theButton;
	Prototype theMethod;
	Arguments theArguments;
	bool theRepeat;

public:
	// load/save convenience functions
	const IRAction &loadFromConfig(KConfig &theConfig, int index);
	void saveToConfig(KConfig &theConfig, int index) const;

	// may be changed to a profile-based representation in the future.
	const QString function() const { return theObject + "::" + theMethod.prototype(); }
	const QString application() const { return theProgram; }

	// bog standard raw DCOP stuff
	const QString &program() const { return theProgram; }
	const QString &object() const { return theObject; }
	const Prototype &method() const { return theMethod; }
	const QString &remote() const { return theRemote; }
	const QString &button() const { return theButton; }
	const Arguments &arguments() const { return theArguments; }
	const bool repeat() const { return theRepeat; }

	void setProgram(const QString &newProgram) { theProgram = newProgram; }
	void setObject(const QString &newObject) { theObject = newObject; }
	void setMethod(const Prototype &newMethod) { theMethod = newMethod; }
	void setRemote(const QString &newRemote) { theRemote = newRemote; }
	void setButton(const QString &newButton) { theButton = newButton; }
	void setArguments(const Arguments &newArguments) { theArguments = newArguments; }
	void setRepeat(bool newRepeat) { theRepeat = newRepeat; }

	IRAction(const QString &newProgram, const QString &newObject, const QString &newMethod, const Arguments &newArguments, const QString &newRemote, const QString &newButton, bool newRepeat);
	IRAction() { theProgram = QString::null; };
};

#endif
