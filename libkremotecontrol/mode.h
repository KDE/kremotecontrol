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

#ifndef MODE_H
#define MODE_H

#include "kremotecontrol_export.h"

#include <QString>
#include <qmetatype.h>


class KREMOTECONTROL_EXPORT Mode
{
  public:
    Mode();
    Mode(const QString &name, const QString &remote, const QString &iconName = QString());
    
    QString name() const;
    QString remote() const;
    QString iconName() const;
    
    void setName(const QString &name);
    void setRemote(const QString &remote);
    void setIconName(const QString &iconName);

    bool operator==(const Mode &mode) const; 
    
  protected:
    QString m_name;
    QString m_remote;
    QString m_iconName;
};

Q_DECLARE_METATYPE(Mode)

#endif
