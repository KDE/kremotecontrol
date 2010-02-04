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

#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include "argument.h"
#include "kremotecontrol_export.h"

#include <qmetatype.h>

class KREMOTECONTROL_EXPORT Prototype
{
  public:
    Prototype(const QString &name, QList<Argument> args = QList<Argument>());
    Prototype();
    
    QString name() const;
    QList<Argument> args() const;
    void setArgs(const QList<Argument> &args);
    
    bool operator==(const Prototype &other) const;
    
  private:
    QString m_name;
    QList<Argument> m_args;
};

Q_DECLARE_METATYPE(Prototype)

#endif // PROTOTYPE_H
