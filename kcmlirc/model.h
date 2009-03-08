
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
 *
 *  Created on: 01.02.2009
 *      @author Frank Scheffold
 */

#ifndef MODEL_H_
#define MODEL_H_
#include <QStringListModel>

//class QVariant
//class QString
#endif /* MODEL_H_ */

class DBusProfileModel : public  QStringListModel
{

public:

  DBusProfileModel (QObject * parent );

  virtual QVariant data( const QModelIndex & index, int  role) const;
  static QString trimAppname(const QString &appName);
  static bool decendingLessThan(const QPair<QString, int> &s1, const QPair<QString, int> &s2);
  static bool ascendingLessThan(const QPair<QString, int> &s1, const QPair<QString, int> &s2);
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);


};


