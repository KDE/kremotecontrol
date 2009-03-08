/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


/**
  * @author Gav Wood
  */

#ifndef IRACTION_H
#define IRACTION_H



#include "prototype.h"
#include "arguments.h"
#include "profileserver.h"

#include <kdebug.h>

/**
@author Gav Wood
*/

class KConfig;

class IRAction
{
    QString theProgram, theObject, theRemote, theButton, theMode;
    Prototype theMethod;
    Arguments theArguments;
    bool theRepeat, theAutoStart, theDoBefore, theDoAfter;
    IfMulti theIfMulti;
    bool theUnique;

public:
    // load/save convenience functions
    static IRAction *loadFromConfig(KConfig &theConfig, int index);
    void saveToConfig(KConfig &theConfig, int index) const;

    // may be changed to a profile-based representation in the future.
    const QString function() const;
    const QString application() const;
    const QString buttonName() const;
    const QString remoteName() const;
    const QString notes() const;

    // bog standard raw DCOP stuff
    const QString &program() const {
        return theProgram;
    }
    const QString &object() const {
        return theObject;
    }
    const Prototype &method() const {
        return theMethod;
    }
    const QString &remote() const {
        return theRemote;
    }
    const QString &mode() const {
        return theMode;
    }
    const QString &button() const {
        return theButton;
    }
    const Arguments arguments() const {
        if (theProgram.isEmpty() && theObject.isEmpty()) {
          return Arguments();
        }else{
          return theArguments;
        }
    }
    bool repeat() const {
        return theRepeat;
    }
    bool autoStart() const {
        return theAutoStart;
    }
    IfMulti ifMulti() const {
        return theIfMulti;
    }
    bool unique() const {
        return theUnique;
    }

    const QString &modeChange() const {
        return theObject;
    }
    bool doBefore() const {
        return theDoBefore;
    }
    bool doAfter() const {
        return theDoAfter;
    }

    bool isModeChange() const {
        return theProgram.isEmpty();
    }
    bool isJustStart() const {
        return (!theProgram.isEmpty()) && theObject.isEmpty();
    }

    void setProgram(const QString &newProgram) {
        theProgram = newProgram;
    }
    void setObject(const QString &newObject) {
        theObject = newObject;
    }
    void setMethod(const Prototype &newMethod) {
        theMethod = newMethod;
    }
    void setRemote(const QString &newRemote) {
        theRemote = newRemote;
    }
    void setMode(const QString &newMode) {
        theMode = newMode;
    }
    void setButton(const QString &newButton) {
        theButton = newButton;
    }
    void setArguments(const Arguments &newArguments) {
        theArguments = newArguments;
    }
    void setRepeat(bool newRepeat) {
        theRepeat = newRepeat;
    }
    void setDoBefore(bool a) {
        theDoBefore = a;
    }
    void setDoAfter(bool a) {
        theDoAfter = a;
    }
    void setAutoStart(bool newAutoStart) {
        theAutoStart = newAutoStart;
    }
    void setModeChange(const QString &a) {
        theObject = a;
    }
    void setIfMulti(const IfMulti a) {
        theIfMulti = a;
    }
    void setUnique(const bool a) {
        theUnique = a;
    }

    IRAction(const QString &newProgram, const QString &newObject, const QString &newMethod, const Arguments &newArguments, const QString &newRemote, const QString &newMode, const QString &newButton, const bool newRepeat, const bool newAutoStart, const bool newDoBefore, const bool newDoAfter, const bool newUnique, const IfMulti newIfMulti);
    
    IRAction() {
        theProgram.clear();
    };

};

#endif
