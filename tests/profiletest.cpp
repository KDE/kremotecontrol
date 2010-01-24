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

void ProfileTest::testVersionString(){
  NewProfile profile("id1", "name", "1.1","some author", "dummy 1" );
  QVERIFY(profile.version() == "1.1");
  profile =NewProfile("id1", "name", "0.1","some author", "dummy 1" );
  QVERIFY(profile.version() == "0.1");
  profile = NewProfile("id1", "name", "1.0","some author", "dummy 1" );
  QVERIFY(profile.version() == "1.0");
}

void ProfileTest::testVersion()
{
  NewProfile profile_1("id1", "name", "1.1","some author", "dummy 1" );
  NewProfile profile_2("id2", "name", "1.1","some author", "dummy 1" );
  
  QCOMPARE(0, profile_1.compareVersion(profile_2) );
  
  profile_2=NewProfile("id2", "name", "1.0","some author", "dummy 1" );
  QCOMPARE(1, profile_1.compareVersion(profile_2) );

  profile_2 =NewProfile("id2", "name", "1.2","some author", "dummy 1" );
  QCOMPARE(-1, profile_1.compareVersion(profile_2) );

  profile_2 =NewProfile("id2", "name", "2.0","some author", "dummy 1" );
  QCOMPARE(-1, profile_1.compareVersion(profile_2) );
}

QTEST_MAIN( ProfileTest )

