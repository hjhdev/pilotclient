/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "tool.h"
#include "blackcore/context_runtime.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackmisc/icons.h"
#include "blackmisc/networkutils.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/project.h"
#include "blackmisc/loghandler.h"
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>
#include <QMetaMethod>
#include <QApplication>
#include <QIcon>
#include <QTextStream>

/*!
 * DBus tests, tests marshalling / unmarshalling of many value classes.
 * Forks two processes and sends data via DBus among them.
 */
int main(int argc, char *argv[])
{
    // metadata are registered in runtime
    QApplication a(argc, argv); // not QCoreApplication because of icon, http://qt-project.org/forums/viewthread/15412
    CLogHandler::instance()->install();
    CLogHandler::instance()->enableConsoleOutput(false); //! \todo How can I change the level only and display info and above?
    QIcon icon(BlackMisc::CIcons::swiftNova24());
    QApplication::setWindowIcon(icon);
    QTextStream cin(stdin);
    QTextStream cout(stdout);

    cout << BlackMisc::CProject::version();
    cout << BlackMisc::CProject::compiledInfo();
    cout << endl;

    cout << "1 + la/ra .. session DBus server (default)" << endl;
    cout << "2 + la/ra .. system DBus server" << endl;
    cout << "3 + la/ra .. P2P DBus server" << endl;
    cout << "la .. local audio, audio runs in this core here (default)" << endl;
    cout << "ra .. remote audio, audio runs in the GUI or elsewhere" << endl;
    cout << "x  .. exit" << endl;

    QString input = cin.readLine().toLower().trimmed();

    // configure DBus server
    QString dBusAddress = BlackCore::CDBusServer::sessionDBusServer();
    if (input.startsWith("2"))
    {
        dBusAddress = BlackCore::CDBusServer::systemDBusServer();
    }
    else if (input.startsWith("3"))
    {
        cout << "found: " << BlackMisc::CNetworkUtils::getKnownIpAddresses().join(' ') << endl;
        cout << "enter ip/port, e.g. 127.0.0.1:45000 (default)" << endl;
        dBusAddress = cin.readLine().toLower();
        dBusAddress = BlackCore::CDBusServer::p2pAddress(dBusAddress);
    }
    else if (input.startsWith("x"))
    {
        return 0;
    }

    // with remote audio
    bool remoteAudio = input.contains("ra");
    BlackCore::CRuntime *core = remoteAudio ?
                                new BlackCore::CRuntime(BlackCore::CRuntimeConfig::forCoreAllLocalInDBusNoAudio(dBusAddress), &a) :
                                new BlackCore::CRuntime(BlackCore::CRuntimeConfig::forCoreAllLocalInDBus(dBusAddress), &a);

    // tool to allow input indepent from event loop
    QtConcurrent::run(BlackMiscTest::Tool::serverLoop, core); // QFuture<void> future
    cout << "Server event loop, pid:" << BlackMiscTest::Tool::getPid() << "Thread id:" << QThread::currentThreadId();

    // end
    return a.exec();
}
