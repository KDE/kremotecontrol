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

#ifndef NEWPROFILEACTION_H
#define NEWPROFILEACTION_H

#include "dbusaction.h"
#include "kremotecontrol_export.h"

class KREMOTECONTROL_EXPORT ProfileAction : public DBusAction
{
  public:
    ProfileAction(const QString& button, const Mode &mode, const QString &profile, const QString &actionTemplate);

    QString profileName() const;
    QString actionTemplateID() const;
    
    virtual void operator=(const ProfileAction &action);
    virtual bool operator==(const ProfileAction &other) const;
    
    virtual QString name() const;
    virtual QString description() const;
    
  protected:
    QString m_profileName;
    QString m_template;
};

#endif // PROFILEACTION_H
