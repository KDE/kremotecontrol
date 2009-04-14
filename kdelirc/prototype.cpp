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

#include "prototype.h"
#include <kdebug.h>

#include <QRegExp>

KDE_EXPORT Prototype::Prototype(const Prototype &pType)
{
    original = pType.prototype();
    parse();

}

KDE_EXPORT Prototype::Prototype(const QString &source)
{
    original = source;
    parse();
}

KDE_EXPORT Prototype::~Prototype()
{
}

KDE_EXPORT const QString Prototype::argumentList() const
{
    QString ret = "";
    for (int i = 0; i < theTypes.count(); i++)
        ret += (i ? ", " : "") + theTypes[i] + ' ' + theNames[i];
    return ret;
}

const QString Prototype::argumentListNN() const
{
    QString ret = "";
    for (int i = 0; i < theTypes.count(); i++)
        ret += (i ? ", " : "") + theTypes[i];
    return ret;
}

void Prototype::parse()
{
    theNames.clear();
    theTypes.clear();

    QRegExp main("^(.*) (\\w[\\d\\w]*)\\((.*)\\)");
    QRegExp parameters("^\\s*([^,\\s]+)(\\s+(\\w[\\d\\w]*))?(,(.*))?$");
    if (main.indexIn(original) == -1) return;
    theReturn = main.cap(1);    
     theReturnType = QVariant(main.cap(1)).type();  

    theName = main.cap(2);
    QString args = main.cap(3);
    while (parameters.indexIn(args) != -1) {
        theTypes += parameters.cap(1);
        theNames += parameters.cap(3);
        argumentTypes.append(QPair<QVariant::Type, QString>(QVariant::nameToType(parameters.cap(1).toLocal8Bit()), parameters.cap(3)));
        args = parameters.cap(5);
      
    }
//  kDebug() << "arguments " << argumentTypes;
}

KDE_EXPORT bool Prototype::operator==(const Prototype& other) const {
    return this->prototype() == other.prototype();
}


KDE_EXPORT bool Prototype::operator>(const Prototype& other) const {
    return this->prototype() > other.prototype();
}


KDE_EXPORT bool Prototype::operator<(const Prototype& other) const {
    return this->prototype() < other.prototype();
}

