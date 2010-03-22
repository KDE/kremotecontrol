/*
    Copyright (C) <2009>  <Frank Scheffold (fscheffold@googlemail.com)>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "executionengine.h"
#include "dbusactionexecutor.h"
#include "kremotecontrol_export.h"

#include <kglobal.h>
#include <kdebug.h>

#include <QtCore/QMap>

class ExecutionEnginePrivate
{
    public:
        ExecutionEnginePrivate();
        
        QMap<Action::ActionType, ActionExecutor*> executors;
  
};

K_GLOBAL_STATIC(ExecutionEnginePrivate, executionEnginePrivate)

ExecutionEnginePrivate::ExecutionEnginePrivate() {
    DBusActionExecutor *dbusExecutor = new DBusActionExecutor();
    executors.insert(Action::DBusAction, dbusExecutor);
    // ProfileActions are DBusActions with additional Information. They can be handled by the DBusExecutor
    executors.insert(Action::ProfileAction, dbusExecutor);
}

void KREMOTECONTROL_EXPORT ExecutionEngine::executeAction(Action* action) {
    ActionExecutor *executor = executionEnginePrivate->executors.value(action->type());
    if(executor != 0){
        executor->execute(action);
    } else {
        kDebug() << "No Executor found for actiontype" << action->type();
    }
}
