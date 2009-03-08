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

#include <QtAlgorithms>
#include <kdebug.h>
#include <QVariant>
#include "model.h"

DBusProfileModel::DBusProfileModel(QObject *parent = 0) :
  QStringListModel(parent)
{
kdDebug() << "new dbus list model";
}

bool DBusProfileModel::ascendingLessThan(const QPair<QString, int> &s1, const QPair<QString, int> &s2)
{
    return trimAppname(s1.first) < trimAppname(s2.first);
}

bool DBusProfileModel::decendingLessThan(const QPair<QString, int> &s1, const QPair<QString, int> &s2)
{
    return trimAppname(s1.first) > trimAppname(s2.first);
}

QString DBusProfileModel::trimAppname(const QString &appName)
{

  int lastIndex = appName .lastIndexOf(".") + 1;
  if (lastIndex < appName.size()) {
    QString s = appName;
    QString domainName = appName;
    s.remove(0, lastIndex);
    domainName.remove(lastIndex -1, domainName.length());
    return  s.left(1).toUpper() + s.right(s.size() - 1) + " (" + domainName+")";
  }
  return appName;
}

QVariant DBusProfileModel::data(const QModelIndex & index, int role = Qt::DisplayRole) const
{
  if (!index.isValid()) {
      kDebug() << " Index invalid";
    return QVariant();
  }

  if (index.row() >= 0 || index.row() <= stringList().size()) {
    if (role == Qt::DisplayRole || role == Qt::EditRole) {

      return trimAppname(stringList().at(index.row()));
    }
    if (role == Qt::UserRole) {

      return stringList().at(index.row());
    }
  }        
  return QVariant();

}


/*!
  // Copy and pasted from QStringListModel::sort() to overwrite ascendingLess and descendingLess
*/
void  DBusProfileModel::sort(int, Qt::SortOrder order)
{
  kDebug()<< "sorting";
  emit layoutAboutToBeChanged();

    QList<QPair<QString, int> > list;
    for (int i = 0; i < stringList().count(); ++i)
        list.append(QPair<QString, int>(stringList().at(i), i));

    if (order == Qt::AscendingOrder)
        qSort(list.begin(), list.end(), DBusProfileModel::ascendingLessThan);
    else
        qSort(list.begin(), list.end(), DBusProfileModel::decendingLessThan);

    stringList().clear();
    QVector<int> forwarding(list.count());
    for (int i = 0; i < list.count(); ++i) {
        stringList().append(list.at(i).first);
        forwarding[list.at(i).second] = i;
    }

    QModelIndexList oldList = persistentIndexList();
    QModelIndexList newList;
    for (int i = 0; i < oldList.count(); ++i)
        newList.append(index(forwarding.at(oldList.at(i).row()), 0));
    changePersistentIndexList(oldList, newList);

    emit layoutChanged();


}
