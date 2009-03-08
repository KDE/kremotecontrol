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
/*
 * dbusinterface.h
 *
 *  Created on: 14.02.2009
 *      Author: Michael Zanetti
 */

#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include "prototype.h"
#include "iraction.h"

#include <QStringList>

class DBusInterface
{

private:
  DBusInterface();

  bool searchForProgram(const IRAction &action, QStringList &programs);

public:


  ~DBusInterface();

  static DBusInterface *getInstance();

  bool isProgramRunning(const QString &program);

  QStringList getRegisteredPrograms();
  QStringList getObjects(const QString &program);
  QList<Prototype> getFunctions(const QString &program, const QString &object);
  
  void executeAction(const IRAction &action);

};

#endif
