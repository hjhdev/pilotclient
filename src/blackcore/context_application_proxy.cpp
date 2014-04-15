/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_application_proxy.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    CContextApplicationProxy::CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContextApplicationBase(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(serviceName , IContextApplication::ObjectPath(), IContextApplication::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals
     */
    void CContextApplicationProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        // signals originating from impl side
        connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                           "statusMessage", this, SIGNAL(statusMessage(BlackMisc::CStatusMessage)));
        connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                           "statusMessages", this, SIGNAL(statusMessages(BlackMisc::CStatusMessageList)));
        connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                           "redirectedOutput", this, SIGNAL(redirectedOutput(BlackMisc::CStatusMessage, qint64)));
        connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                           "componentChanged", this, SIGNAL(componentChanged(uint, uint)));
    }

    /*
     * Ping, is DBus alive?
     */
    qint64 CContextApplicationProxy::ping(qint64 token) const
    {
        qint64 t = this->m_dBusInterface->callDBusRet<qint64>(QLatin1Literal("ping"), token);
        return t;
    }

    /*
     * Status messages
     */
    void CContextApplicationProxy::sendStatusMessage(const BlackMisc::CStatusMessage &message)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("sendStatusMessage"), message);
    }

    /*
     * Status messages
     */
    void CContextApplicationProxy::sendStatusMessages(const BlackMisc::CStatusMessageList &messages)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("sendStatusMessages"), messages);
    }

    /*
     * Component has changed
     */
    void CContextApplicationProxy::notifyAboutComponentChange(uint component, uint action)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("notifyAboutComponentChange"), component, action);
    }

    /*
     * To file
     */
    bool CContextApplicationProxy::writeToFile(const QString &fileName, const QString &content)
    {
        if (fileName.isEmpty()) return false;
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("writeToFile"), fileName, content);
    }

    /*
     * From file
     */
    QString CContextApplicationProxy::readFromFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return "";
        return this->m_dBusInterface->callDBusRet<QString>(QLatin1Literal("readFromFile"), fileName);
    }

    /*
     *  Delete file
     */
    bool CContextApplicationProxy::removeFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return false;
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("deleteFile"), fileName);
    }
} // namespace
