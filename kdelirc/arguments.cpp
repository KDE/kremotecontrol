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


#include "arguments.h"

KDE_EXPORT Arguments::Arguments()
{
}

KDE_EXPORT Arguments::Arguments(const Arguments &args) {
    arguments = args.getArgumentsList();
}

KDE_EXPORT Arguments::~Arguments()
{
}

KDE_EXPORT QList<QVariant> Arguments::getArgumentsList() const {
    return arguments;
}

KDE_EXPORT void Arguments::operator=(const Arguments& args) {
    arguments = args.getArgumentsList();
}


KDE_EXPORT const QString Arguments::toString() const
{
    QString ret = "";
    for (QList<QVariant>::const_iterator i = arguments.constBegin(); i != arguments.constEnd(); ++i) {
        QString s = (*i).toString();
        if (s.isNull() || s.isEmpty()) s = "...";
        if (i != arguments.constBegin()) ret += ", ";
        ret += s;
    }
    return ret;
}

KDE_EXPORT void Arguments::clear() {
    arguments.clear();
}

KDE_EXPORT void Arguments::append(const QVariant& arg) {
    arguments.append(arg);
}

KDE_EXPORT int Arguments::count() const {
    return arguments.count();
}

KDE_EXPORT QVariant Arguments::at(int i) const {
    return arguments.at(i);
}

KDE_EXPORT QVariant Arguments::back() {
    return arguments.back();
}

KDE_EXPORT int Arguments::size() const {
    return arguments.size();
}

