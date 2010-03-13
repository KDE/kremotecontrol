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

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <kdemacros.h>

#include <QtCore/QVariant>

class KDE_EXPORT Argument
{
    public:
        Argument();
        Argument(const QVariant &value);
        Argument(const QVariant &value, const QString &description);
        
        QVariant value() const;
        void setValue(const QVariant &value);
        
        QString description() const;
        void setDescription(const QString &description);
        
        bool operator==(const Argument &other) const;
      
    protected:
        QVariant m_value;
        QString m_description;
};

Q_DECLARE_METATYPE(Argument)

#endif // ARGUMENT_H
