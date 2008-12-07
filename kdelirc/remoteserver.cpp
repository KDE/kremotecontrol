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

#include "remoteserver.h"

#include <QFile>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>

RemoteServer *RemoteServer::theInstance = 0;

RemoteServer::RemoteServer()
{
    loadRemotes();
}

RemoteServer::~RemoteServer()
{
}

void RemoteServer::loadRemotes()
{
    QStringList theFiles = KGlobal::dirs()->findAllResources("data", "remotes/*.remote.xml");
    for (QStringList::iterator i = theFiles.begin(); i != theFiles.end(); ++i) {
        kDebug() << "Found data file: " << *i ;
        Remote *p = new Remote();
        p->loadFromFile(*i);
        theRemotes.insert(p->id(), p);
    }
}

Remote::Remote()
{
}

Remote::~Remote()
{
}

void Remote::loadFromFile(const QString &fileName)
{
    charBuffer = "";
    curRB = 0;

    QFile xmlFile(fileName);
    QXmlInputSource source(&xmlFile);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.parse(source);
}

bool Remote::characters(const QString &data)
{
    charBuffer += data;
    return true;
}

bool Remote::startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes)
{
    if (name == "remote")
        theId = theName = attributes.value("id");
    else if (name == "button") {
        curRB = new RemoteButton();
        curRB->setId(attributes.value("id"));
        curRB->setClass(attributes.value("id"));
        if (attributes.index("class") > -1)
            curRB->setClass(attributes.value("class"));
        curRB->setParameter(attributes.value("parameter"));
        curRB->setName(attributes.value("id"));
    }

    charBuffer = "";
    return true;
}

bool Remote::endElement(const QString &, const QString &, const QString &name)
{
    if (name == "name")
        if (curRB)
            curRB->setName(charBuffer);
        else
            theName = charBuffer;
    else if (name == "author")
        theAuthor = charBuffer;
    else if (name == "button") {
        theButtons.insert(curRB->id(), curRB);
        curRB = 0;
    }

    charBuffer = "";
    return true;
}
