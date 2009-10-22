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
#include <solid/control/remotecontrolbutton.h>


/**
@author Gav Wood
*/


using namespace Solid::Control;
class KConfig;

class IRAction
{
  
 
  
    QString theProgram, theObject, theMode;
    Prototype theMethod;
    Arguments theArguments;
    bool theRepeat, theAutoStart, theDoBefore, theDoAfter;
    IfMulti theIfMulti;
    bool theUnique;
    RemoteControlButton theRemoteButton;
  

public:
    // load/save convenience functions
    static IRAction *loadFromConfig(KConfig &theConfig, int index);
    void saveToConfig(KConfig &theConfig, int index) const;

    
     const QString application() const;
    // may be changed to a profile-based representation in the future.
    const QString function() const;

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
  
    const QString &mode() const {
        return theMode;
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
    
    void setMode(const QString &newMode) {
        theMode = newMode;
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

    const RemoteControlButton  getButton() const{
    return theRemoteButton;
    }
    
    void setButton(const RemoteControlButton &button){
   theRemoteButton= button;
 }
  
    IRAction( const RemoteControlButton button) :   theRemoteButton(button) {
       theProgram.clear();
    }

    IRAction( QString remote, QString button) :   theRemoteButton(remote, button,0) {
       theProgram.clear();
    };
    
    QString button(){
	  return theRemoteButton.name();
    }
    
    QString remote(){
      return theRemoteButton.remoteName();
    }
};

#endif
