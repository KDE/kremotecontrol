/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <2009>  <Frank Scheffold (fscheffold@googlemail.com)>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "actiondelegator.h"

#include <kdebug.h>

ActionDelegator::ActionDelegator()
{
  registerAction(Action::staticMetaObject, new ActionExcecutor());
}



void ActionDelegator::registerAction( const QMetaObject metaObject,   ActionExecutor* actionExecutor)
{
  kDebug() << "metaObject " << QString(metaObject.superClass()->className()) ;
  kDebug()<< "super iraction class " << QString(Action::staticMetaObject.className());
  
  if(QString(metaObject.superClass()->className())   == QString(Action::staticMetaObject.className())){  
    executors.insert(QString(metaObject.className()), actionExecutor);
  }else{
    kDebug() << "class is no superclass of iraction";
 }
}

void ActionDelegator::executeAction(const Action* action)
{
 ActionExecutor * executor = executors.value(QString(action->metaObject()->className()));
 if(executor != 0){
   executor->execute(action);
 }
}
