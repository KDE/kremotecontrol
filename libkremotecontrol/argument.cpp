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

#include "argument.h"

Argument::Argument()
{

}

Argument::Argument(const QVariant& value)
{
  m_value = value;
}

Argument::Argument(const QVariant& value, const QString& description)
{
  m_value = value;
  m_description = description;
}

QVariant Argument::value() const
{
  return m_value;
}

void Argument::setValue(const QVariant& value) {
  m_value = value;
}

QString Argument::description() const
{
  return m_description;
}

void Argument::setDescription(const QString& description) {
  m_description = description;
}

bool Argument::operator==(const Argument& other) const
{
  return m_value == other.value() &&
	  m_description == other.description();
}

