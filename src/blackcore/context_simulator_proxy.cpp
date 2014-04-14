/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_simulator_proxy.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackCore
{

    CContextSimulatorProxy::CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextSimulator(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextSimulator::ObjectPath(), IContextSimulator::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    void CContextSimulatorProxy::relaySignals(const QString &/*serviceName*/, QDBusConnection &/*connection*/)
    { }

    bool CContextSimulatorProxy::isConnected() const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
    }

    BlackMisc::Aviation::CAircraft CContextSimulatorProxy::getOwnAircraft() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraft>(QLatin1Literal("getOwnAircraft"));
    }

    BlackSim::CSimulatorInfo CContextSimulatorProxy::getSimulatorInfo() const
    {
        return m_dBusInterface->callDBusRet<BlackSim::CSimulatorInfo>(QLatin1Literal("getSimulatorInfo"));
    }

} // namespace BlackCore
