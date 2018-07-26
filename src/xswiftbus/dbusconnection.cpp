#include <utility>

/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbusconnection.h"
#include "dbusobject.h"

#include <algorithm>
#include <cassert>
#include <memory>

namespace XSwiftBus
{

    CDBusConnection::CDBusConnection()
    {
        dbus_threads_init_default();
    }

    CDBusConnection::CDBusConnection(DBusConnection *connection)
    {
        dbus_connection_ref(connection);
        // Don't exit application, if the connection is disconnected
        dbus_connection_set_exit_on_disconnect(connection, false);
        m_connection.reset(connection);
    }

    CDBusConnection::~CDBusConnection()
    {
        close();
        if (m_connection) { dispatch(); }
        if (m_dispatcher) { m_dispatcher->remove(this); }
    }

    bool CDBusConnection::connect(BusType type)
    {
        assert(type == SessionBus);
        DBusError error;
        dbus_error_init(&error);

        DBusBusType dbusBusType;
        switch (type)
        {
        case SessionBus: dbusBusType = DBUS_BUS_SESSION; break;
        }

        m_connection.reset(dbus_bus_get_private(dbusBusType, &error));
        if (dbus_error_is_set(&error))
        {
            m_lastError = CDBusError(&error);
            return false;
        }

        // Don't exit application, if the connection is disconnected
        dbus_connection_set_exit_on_disconnect(m_connection.get(), false);
        return true;
    }

    void CDBusConnection::setDispatcher(CDBusDispatcher *dispatcher)
    {
        assert(dispatcher);

        m_dispatcher = dispatcher;

        m_dispatcher->add(this);

        dbus_connection_set_watch_functions(
            m_connection.get(),
            dispatcher->m_watchCallbacks.add,
            dispatcher->m_watchCallbacks.remove,
            dispatcher->m_watchCallbacks.toggled,
            &dispatcher->m_watchCallbacks, nullptr);

        dbus_connection_set_timeout_functions(
            m_connection.get(),
            dispatcher->m_timeoutCallbacks.add,
            dispatcher->m_timeoutCallbacks.remove,
            dispatcher->m_timeoutCallbacks.toggled,
            &dispatcher->m_timeoutCallbacks, nullptr);
    }

    void CDBusConnection::requestName(const std::string &name)
    {
        DBusError error;
        dbus_error_init(&error);
        dbus_bus_request_name(m_connection.get(), name.c_str(), 0, &error);
    }

    bool CDBusConnection::isConnected() const
    {
        return m_connection && dbus_connection_get_is_connected(m_connection.get());
    }

    void CDBusConnection::registerObjectPath(CDBusObject *object, const std::string &interfaceName, const std::string &objectPath, const DBusObjectPathVTable &dbusObjectPathVTable)
    {
        (void) interfaceName;
        if (!m_connection) { return; }

        dbus_connection_try_register_object_path(m_connection.get(), objectPath.c_str(), &dbusObjectPathVTable, object, nullptr);
    }

    void CDBusConnection::sendMessage(const CDBusMessage &message)
    {
        if (!isConnected()) { return; }
        dbus_uint32_t serial = message.getSerial();
        dbus_connection_send(m_connection.get(), message.m_message, &serial);
    }

    void CDBusConnection::close()
    {
        if (m_connection) { dbus_connection_close(m_connection.get()); }
    }

    void CDBusConnection::dispatch()
    {
        dbus_connection_ref(m_connection.get());
        if (dbus_connection_get_dispatch_status(m_connection.get()) == DBUS_DISPATCH_DATA_REMAINS)
        {
            while (dbus_connection_dispatch(m_connection.get()) == DBUS_DISPATCH_DATA_REMAINS);
        }
        dbus_connection_unref(m_connection.get());
    }

    void CDBusConnection::setDispatchStatus(DBusConnection *connection, DBusDispatchStatus status)
    {
        if (dbus_connection_get_is_connected(connection) == FALSE) { return; }

        switch (status)
        {
        case DBUS_DISPATCH_DATA_REMAINS:
            //m_dispatcher->add(this);
            break;
        case DBUS_DISPATCH_COMPLETE:
        case DBUS_DISPATCH_NEED_MEMORY:
            break;
        }
    }

    void CDBusConnection::setDispatchStatus(DBusConnection *connection, DBusDispatchStatus status, void *data)
    {
        auto *obj = static_cast<CDBusConnection *>(data);
        return obj->setDispatchStatus(connection, status);
    }

}
