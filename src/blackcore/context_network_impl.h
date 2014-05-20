/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_IMPL_H
#define BLACKCORE_CONTEXTNETWORK_IMPL_H

#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_runtime.h"
#include "blackcore/dbus_server.h"
#include "blackcore/network.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/setnetwork.h"
#include "blackmisc/nwclientlist.h"

#include <QMap>

class QTimer;

namespace BlackCore
{
    class CVatsimBookingReader;
    class CVatsimDataFileReader;

    //! Network context implementation
    class CContextNetwork : public IContextNetwork
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)
        friend class CRuntime;

    public:
        //! Destructor
        virtual ~CContextNetwork();

        /*!
         * Set own aircraft
         * \param aircraft
         * \return message list, as aircraft can only be set prior connecting
         */
        BlackMisc::CStatusMessageList setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        //! Own position, be aware height is terrain height
        void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude);

        //! Complete situation update
        void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Update own cockpit
        void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

        //! Get own aircraft
        BlackMisc::Aviation::CAircraft getOwnAircraft() const;

    public slots: // IContextNetwork overrides

        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override;

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsOnline;
        }

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override
        {
            // this->log(Q_FUNC_INFO);
            return m_atcStationsBooked;
        }

        //! \copydoc IContextNetwork::getAircraftsInRange()
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const override
        {
            // this->log(Q_FUNC_INFO);
            return m_aircraftsInRange;
        }

        //! \copydoc IContextNetwork::connectToNetwork()
        virtual BlackMisc::CStatusMessageList connectToNetwork(uint mode) override;

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessageList disconnectFromNetwork() override;

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override;

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;

        //! \copydoc IContextNetwork::getMetar()
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode) override;

        //! \copydoc IContextNetwork::getSelectedVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override;

        //! \copydoc IContextNetwork::getSelectedAtcStations
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;

        //! \copydoc IContextNetwork::getUsers()
        virtual BlackMisc::Network::CUserList getUsers() const override;

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override;

        //! \copydoc IContextNetwork::getUsersForCallsign
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getOtherClients
        virtual BlackMisc::Network::CClientList getOtherClients() const override;

        //! \copydoc IContextNetwork::getOtherClientForCallsigns
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override;

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override;

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override;

    protected:
        //! Constructor, with link to runtime
        CContextNetwork(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextNetwork *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextNetwork::ObjectPath(), this);
            return this;
        }

    private:
        BlackMisc::Aviation::CAtcStationList m_atcStationsOnline;
        BlackMisc::Aviation::CAtcStationList m_atcStationsBooked;
        BlackMisc::Aviation::CAircraftList m_aircraftsInRange;
        BlackMisc::Network::CClientList m_otherClients;
        BlackCore::INetwork *m_network;
        QMap<QString, BlackMisc::Aviation::CInformationMessage> m_metarCache /*!< Keep METARs for a while */;

        // for reading XML and VATSIM data files
        CVatsimBookingReader *m_vatsimBookingReader;
        CVatsimDataFileReader *m_vatsimDataFileReader;
        QTimer *m_dataUpdateTimer; //!< general updates such as ATIS, frequencies, see requestDataUpdates()

        //! Replace value by new values
        void setAtcStationsBooked(const BlackMisc::Aviation::CAtcStationList &newStations);

        //! Replace value by new values
        void setAtcStationsOnline(const BlackMisc::Aviation::CAtcStationList &newStations);

        //! The "central" ATC list with online ATC controllers
        BlackMisc::Aviation::CAtcStationList &atcStationsOnline() { return m_atcStationsOnline; }

        //! ATC list, with booked controllers
        BlackMisc::Aviation::CAtcStationList &atcStationsBooked() { return m_atcStationsBooked; }

        //! Init my very own aircraft
        void initOwnAircraft();

        //! Get network settings
        BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getRuntime()->getIContextSettings());
            return this->getRuntime()->getIContextSettings()->getNetworkSettings();
        }

        //! Send FsInn custom package
        void sendFsipiCustomPackage(const BlackMisc::Aviation::CCallsign &recipientCallsign) const;

        //! Send FsInn custom package
        void sendFsipirCustomPackage(const BlackMisc::Aviation::CCallsign &recipientCallsign) const;

        //! Custom package data based on own aircraft / model
        QStringList createFsipiCustomPackageData() const;

        //! Own aircraft
        const BlackMisc::Aviation::CAircraft &ownAircraft() const;

        //! Own aircraft
        BlackMisc::Aviation::CAircraft &ownAircraft();

    private slots:
        //! ATC bookings received
        void psReceivedBookings(BlackMisc::Aviation::CAtcStationList bookedStations);

        //! Data file has been read
        void psDataFileRead();

        /*!
         * \brief Connection status changed?
         * \param from  old status
         * \param to    new status
         */
        void psFsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to, const QString &message);

        //! ATC position update
        void psFsdAtcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);

        /*!
         * \brief Controller disconnected
         * \param callsign  callsign of controller
         */
        void psFsdAtcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        //! ATIS received
        void psFsdAtisQueryReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atisMessage);

        /*!
         * \brief ATIS received (voice room part)
         * \param callsign  station callsign
         * \param url       voice room's URL
         */
        void psFsdAtisVoiceRoomQueryReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);

        /*!
         * \brief ATIS received (logoff time part)
         * \param callsign  station callsign
         * \param zuluTime  UTC time, when controller will logoff
         */
        void psFsdAtisLogoffTimeQueryReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);

        //! METAR received
        void psFsdMetarReceived(const QString &metarMessage);

        //! Realname recevied
        void psFsdRealNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);

        //! Plane ICAO codes received
        void psFsdIcaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData);

        //! Aircraft position update received
        void psFsdAircraftUpdateReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        //! Pilot disconnected
        void psFsdPilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);

        //! Frequency received
        void psFsdFrequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);

        //! Radio text messages received
        void psFsdTextMessageReceived(const BlackMisc::Network::CTextMessageList &messages);

        //! Capabilities received
        void psFsdCapabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);

        //! Custom package
        void psFsdCustomPackageReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &package, const QStringList &data);

        //! Server reply received
        void psFsdServerReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &host);
    };
}

#endif // guard
