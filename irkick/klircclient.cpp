//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: (C) 2002 by Malte Starostik
// Adaption : Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>

#include <QWidget>
#include <qdialog.h>

#include <qtcpsocket.h>
#include <qsocketnotifier.h>
#include <QFile>

#include <kapplication.h>
#include <ksystemtrayicon.h>
#include <kiconloader.h>
#include <kpassivepopup.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kdebug.h>
#include <klocale.h>

//#include <dcopclient.h>
//#include <dcopref.h>

#include "klircclient.h"


KLircClient::KLircClient(QWidget *parent) : QObject(parent), theSocket(0), listIsUpToDate(false)
{
    connectToLirc();
}

bool KLircClient::connectToLirc()
{
    int sock = ::socket(PF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) return false;

    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/dev/lircd");
    if (::connect(sock, (struct sockaddr *)(&addr), sizeof(addr)) == -1) {
        ::close(sock);
        // in case of mandrake...
        strcpy(addr.sun_path, "/tmp/.lircd");
        if (::connect(sock, (struct sockaddr *)(&addr), sizeof(addr)) == -1) {
            ::close(sock);
            return false;
        }
    }

    theSocket = new QTcpSocket();
    theSocket->setSocketDescriptor(sock);
    connect(theSocket, SIGNAL(readyRead()), SLOT(slotRead()));
    connect(theSocket, SIGNAL(connectionClosed()), SLOT(slotClosed()));
    updateRemotes();
    return true;
}

KLircClient::~KLircClient()
{
    if (theSocket)
        delete theSocket;
}

void KLircClient::slotClosed()
{
    delete theSocket;
    theSocket = 0;
    emit connectionClosed();
}

const QStringList KLircClient::remotes() const
{
    QStringList remotes;
    for (QMap<QString, QStringList>::ConstIterator i = theRemotes.begin(); i != theRemotes.end(); ++i) {
        kDebug() << "Remote: " << i.key();
        remotes.append(i.key());
    }
    remotes.sort();
    return remotes;
}

const QStringList KLircClient::buttons(const QString &theRemote) const
{
    return theRemotes[theRemote];
}

void KLircClient::slotRead()
{
    while (theSocket->bytesAvailable()) {
        QString line = readLine();
        if (line == "BEGIN") {
            // BEGIN
            // <command>
            // [SUCCESS|ERROR]
            // [DATA
            // n
            // n lines of data]
            // END
            line = readLine();
            if (line == "SIGHUP") {
                // Configuration changed
                do line = readLine();
                while (!line.isEmpty() && line != "END");
                updateRemotes();
                return;
            } else if (line == "LIST") {
                // remote control list
                if (readLine() != "SUCCESS" || readLine() != "DATA") {
                    do line = readLine();
                    while (!line.isEmpty() && line != "END");
                    return;
                }
                QStringList remotes;
                int count = readLine().toInt();
                for (int i = 0; i < count; ++i)
                    remotes.append(readLine());
                do line = readLine();
                while (!line.isEmpty() && line != "END");
                if (line.isEmpty())
                    return; // abort on corrupt data
                for (QStringList::ConstIterator it = remotes.begin(); it != remotes.end(); ++it)
                    sendCommand("LIST " + *it);
                return;
            } else if (line.left(4) == "LIST") {
                // button list
                if (readLine() != "SUCCESS" || readLine() != "DATA") {
                    do line = readLine();
                    while (!line.isEmpty() && line != "END");
                    return;
                }
                QString remote = line.mid(5);
                QStringList buttons;
                int count = readLine().toInt();
                for (int i = 0; i < count; ++i) {
                    // <code> <name>
                    QString btn = readLine().mid(17);
                    if (btn.isNull()) break;
                    if (btn.startsWith('\'') && btn.endsWith('\''))
                        btn = btn.mid(1, btn.length() - 2);
                    buttons.append(btn);
                }
                theRemotes.insert(remote, buttons);
            }
            do line = readLine();
            while (!line.isEmpty() && line != "END");
            listIsUpToDate = true;
            kDebug() << "Remotes read!";
            emit remotesRead();
        } else {
            // <code> <repeat> <button name> <remote control name>
            line.remove(0, 17); // strip code
            int pos = line.indexOf(' ');
            if (pos < 0) return;
            bool ok;
            int repeat = line.left(pos).toInt(&ok, 16);
            if (!ok) return;
            line.remove(0, pos + 1);

            pos = line.indexOf(' ');
            if (pos < 0) return;
            QString btn = line.left(pos);
            if (btn.startsWith('\'') && btn.endsWith('\''))
                btn = btn.mid(1, btn.length() - 2);
            line.remove(0, pos + 1);
            kDebug() << "Command received!";
            emit commandReceived(line, btn, repeat);
        }
    }
}

void KLircClient::updateRemotes()
{
    listIsUpToDate = false;
    theRemotes.clear();
    sendCommand("LIST");
}

bool KLircClient::isConnected() const
{
    if (!theSocket) return false;
    return theSocket->state() == QTcpSocket::ConnectedState;
}

bool KLircClient::haveFullList() const
{
    return listIsUpToDate;
}

const QString KLircClient::readLine()
{
    if (!theSocket->canReadLine()) {
        theSocket->waitForReadyRead(500);
        if (!theSocket->canReadLine()) { // Still nothing :(
            return QString();
        }
    }
    QString line = theSocket->readLine();
    line.truncate(line.length() - 1);
    return line;
}

void KLircClient::sendCommand(const QString &command)
{
    QByteArray cmd = QFile::encodeName(command + '\n');;
    theSocket->write(cmd);
}


#include "klircclient.moc"
