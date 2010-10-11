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
#include "profile.h"

#include <KDebug>

#include <QtTest/QTest>


void ProfileTest::testGetter() {
  Profile profile(QLatin1String( "id1" ), QLatin1String( "name" ), QLatin1String( "1.1" ),QLatin1String( "some author" ), QLatin1String( "dummy 1" ) );
  QCOMPARE(QLatin1String( "id1"), profile.profileId());
  QCOMPARE(QLatin1String( "name"), profile.name());
  QCOMPARE(QLatin1String( "1.1"), profile.version());
  QCOMPARE(QLatin1String( "some author"), profile.author());
  QCOMPARE(QLatin1String( "dummy 1"), profile.description());

  profile = Profile();
  QCOMPARE(QLatin1String( "0.0"), profile.version());
}

void ProfileTest::testVersion() {
  Profile *profile_1 = new Profile(QLatin1String( "id1" ), QLatin1String( "name" ), QLatin1String( "1.1" ),QLatin1String( "some author" ), QLatin1String( "dummy 1" ) );
  Profile *profile_2 = new Profile(QLatin1String( "id2" ), QLatin1String( "name" ), QLatin1String( "1.1" ),QLatin1String( "some author" ), QLatin1String( "dummy 1" ) );

  QCOMPARE(0, profile_1->compareVersion(profile_2) );

  profile_2=new Profile(QLatin1String( "id2" ), QLatin1String( "name" ), QLatin1String( "1.0" ),QLatin1String( "some author" ), QLatin1String( "dummy 1" ) );
  QCOMPARE(1, profile_1->compareVersion(profile_2) );

  profile_2 = new Profile(QLatin1String( "id2" ), QLatin1String( "name" ), QLatin1String( "1.2" ),QLatin1String( "some author" ), QLatin1String( "dummy 1" ) );
  QCOMPARE(-1, profile_1->compareVersion(profile_2) );
  delete profile_2;

  profile_2 =new Profile(QLatin1String( "id2" ), QLatin1String( "name" ), QLatin1String( "2.0" ),QLatin1String( "some author" ), QLatin1String( "dummy 1" ) );
  QCOMPARE(-1, profile_1->compareVersion(profile_2) );

  delete profile_1;
  delete profile_2;
}

QTEST_MAIN( ProfileTest )

