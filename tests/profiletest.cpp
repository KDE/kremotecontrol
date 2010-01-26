/*
    Copyright [yyyy] [name of copyright owner]

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

#include "profiletest.h"

#include <kdelirc/libkremotecontrol/profile.h>

#include <KDebug>
#include <QTest>


void ProfileTest::testGetter()
{
  Profile profile("id1", "name", "1.1","some author", "dummy 1" );
  QCOMPARE(QString("id1"), profile.profileId());
  QCOMPARE(QString("name"), profile.name());
  QCOMPARE(QString("1.1"), profile.version());
  QCOMPARE(QString("some author"), profile.author());
  QCOMPARE(QString("dummy 1"), profile.description());

  profile = Profile();
  QCOMPARE(QString("0.0"), profile.version());
}

void ProfileTest::testVersion()
{
  Profile profile_1("id1", "name", "1.1","some author", "dummy 1" );
  Profile profile_2("id2", "name", "1.1","some author", "dummy 1" );
  
  QCOMPARE(0, profile_1.compareVersion(profile_2) );
  
  profile_2=Profile("id2", "name", "1.0","some author", "dummy 1" );
  QCOMPARE(1, profile_1.compareVersion(profile_2) );

  profile_2 =Profile("id2", "name", "1.2","some author", "dummy 1" );
  QCOMPARE(-1, profile_1.compareVersion(profile_2) );

  profile_2 =Profile("id2", "name", "2.0","some author", "dummy 1" );
  QCOMPARE(-1, profile_1.compareVersion(profile_2) );
}

QTEST_MAIN( ProfileTest )

