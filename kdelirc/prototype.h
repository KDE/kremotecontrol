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

#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <QStringList>
#include <QPair>
#include <QVariant>


/**
@author Gav Wood
*/

class Prototype;

class Prototype
{
private:
    QString original, theName, theReturn;
    QStringList theNames, theTypes;
    QVariant::Type theReturnType;
    QList<QPair<QVariant::Type, QString> > argumentTypes;
    void parse();

public:
    int count() const {
        return theTypes.count();
    }
    const QPair<QString, QString> operator[](int i) const {
        return qMakePair(theTypes[i], theNames[i]);
    }
    const QString &name(int i) const {
        return theNames[i];
    }
    const QString &type(int i) const {
        return theTypes[i];
    }
    const QString &returnType() const {
        return theReturn;
    }
    const QString &name() const {
        return theName;
    }
    const QString &prototype() const {
        return original;
    }
    const QString argumentList() const;
    const QString argumentListNN() const;
    int argumentCount() const {
        return theTypes.count();
    }
    const QString prototypeNR() const {
        return theName;
    } // + "(" + argumentListNN() + ")"; }

    void setPrototype(const QString &source) {
        original = source;
        parse();
    }
    const QList<QPair<QVariant::Type, QString> >& getArguments() const {
        return argumentTypes;
    }
    QVariant::Type getReturnType() const  {
        return theReturnType;
    }

    Prototype &operator=(const QString &source) {
        setPrototype(source);
        return *this;
    }


    bool operator==(const Prototype &other) const ;
    bool operator<(const Prototype &other) const ;
    bool operator>(const Prototype &other) const ;

    Prototype(const QString &source);
    Prototype() {};
    Prototype(const Prototype &pType);
    ~Prototype();

};

#endif
