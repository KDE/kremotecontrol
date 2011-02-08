/*
    Copyright (C) 2010  Michael Zanetti <michael_zanetti@gmx.net>

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

#ifndef DBUSACTION_H
#define DBUSACTION_H

#include "action.h"
#include "argument.h"
#include "kremotecontrol_export.h"
#include "prototype.h"

#include <remotecontrolbutton.h>

#include <QtCore/QString>

class KREMOTECONTROL_EXPORT DBusAction : public Action
{
    public:
        
        DBusAction(const QString &button);
        DBusAction();
        
        QString application() const;
        void setApplication(const QString &application);
        
        QString node() const;
        void setNode(const QString &node);
        
        Prototype function() const;
        void setFunction(const Prototype &function);
        
        virtual QString name() const;
        virtual QString description() const;
        
        virtual void saveToConfig(KConfigGroup &config);
        virtual void loadFromConfig(const KConfigGroup &config);
        
        virtual Action* clone() const;
    
    protected:
        QString m_application;
        QString m_node;
        Prototype m_function;
};

#endif // DBUSACTION_H
