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
	QString theProgram, theObject, theRemote, theButton, theMode;
	Prototype theMethod;
	Arguments theArguments;
	bool theRepeat, theAutoStart;

public:
	// load/save convenience functions
	const IRAction &loadFromConfig(KConfig &theConfig, int index);
	void saveToConfig(KConfig &theConfig, int index) const;

	// may be changed to a profile-based representation in the future.
	const QString function() const;
	const QString application() const;
	const QString repeatable() const;
	const QString autoStartable() const;
	const QString buttonName() const;
	const QString remoteName() const;

	// bog standard raw DCOP stuff
	const QString &program() const { return theProgram; }
	const QString &object() const { return theObject; }
	const Prototype &method() const { return theMethod; }
	const QString &remote() const { return theRemote; }
	const QString &mode() const { return theMode; }
	const QString &button() const { return theButton; }
	const Arguments &arguments() const { return theArguments; }
	const bool repeat() const { return theRepeat; }
	const bool autoStart() const { return theAutoStart; }

	void setProgram(const QString &newProgram) { theProgram = newProgram; }
	void setObject(const QString &newObject) { theObject = newObject; }
	void setMethod(const Prototype &newMethod) { theMethod = newMethod; }
	void setRemote(const QString &newRemote) { theRemote = newRemote; }
	void setMode(const QString &newMode) { theMode = newMode; }
	void setButton(const QString &newButton) { theButton = newButton; }
	void setArguments(const Arguments &newArguments) { theArguments = newArguments; }
	void setRepeat(bool newRepeat) { theRepeat = newRepeat; }
	void setAutoStart(bool newAutoStart) { theAutoStart = newAutoStart; }

	IRAction(const QString &newProgram, const QString &newObject, const QString &newMethod, const Arguments &newArguments, const QString &newRemote, const QString &newMode, const QString &newButton, bool newRepeat, bool newAutoStart);
	IRAction() { theProgram = QString::null; };
};

#endif
