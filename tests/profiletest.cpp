/*************************************************************************
 * Copyright (C) 2010 by Frank Scheffold <fscheffold@googlemail.com>     *
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

#include "profiletest.h"

#include <kdelirc/libkremotecontrol/profile.h>

#include <KDebug>
#include <QTest>


void ProfileTest::testGetter() {
  Profile profile("id1", "name", "1.1","some author", "dummy 1" );
  QCOMPARE(QString("id1"), profile.profileId());
  QCOMPARE(QString("name"), profile.name());
  QCOMPARE(QString("1.1"), profile.version());
  QCOMPARE(QString("some author"), profile.author());
  QCOMPARE(QString("dummy 1"), profile.description());

  profile = Profile();
  QCOMPARE(QString("0.0"), profile.version());
}

void ProfileTest::testVersion() {
  Profile *profile_1 = new Profile("id1", "name", "1.1","some author", "dummy 1" );
  Profile *profile_2 = new Profile("id2", "name", "1.1","some author", "dummy 1" );

  QCOMPARE(0, profile_1->compareVersion(profile_2) );

  profile_2=new Profile("id2", "name", "1.0","some author", "dummy 1" );
  QCOMPARE(1, profile_1->compareVersion(profile_2) );

  profile_2 = new Profile("id2", "name", "1.2","some author", "dummy 1" );
  QCOMPARE(-1, profile_1->compareVersion(profile_2) );

  profile_2 =new Profile("id2", "name", "2.0","some author", "dummy 1" );
  QCOMPARE(-1, profile_1->compareVersion(profile_2) );

  delete profile_1;
  delete profile_2;
}

QTEST_MAIN( ProfileTest )

