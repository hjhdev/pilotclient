/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRCRAFT_H
#define BLACKMISC_AIRCRAFT_H

#include "nwuser.h"
#include "avaircraftsituation.h"
#include "avaircrafticao.h"
#include "avcallsign.h"
#include "avselcal.h"
#include "aviotransponder.h"
#include "aviocomsystem.h"
#include "valueobject.h"
#include "namevariantpairlist.h"
#include "propertyindex.h"
#include "blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating information of an aircraft
         */
        class CAircraft : public CValueObjectStdTuple<CAircraft>, public Geo::ICoordinateGeodetic
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexCallsign = BlackMisc::CPropertyIndex::GlobalIndexCAircraft,
                IndexPilot,
                IndexDistance,
                IndexCom1System,
                IndexCom2System,
                IndexTransponder,
                IndexSituation,
                IndexIcao
            };

            //! Default constructor.
            CAircraft() : m_distanceToPlane(0, BlackMisc::PhysicalQuantities::CLengthUnit::nullUnit()) {}

            //! Constructor.
            CAircraft(const CCallsign &callsign, const BlackMisc::Network::CUser &user, const CAircraftSituation &situation);

            //! \copydoc CValueObject::toIcon()
            virtual BlackMisc::CIcon toIcon() const override { return this->m_callsign.toIcon(); }

            //! Get callsign.
            const CCallsign &getCallsign() const { return m_callsign; }

            //! Get callsign.
            QString getCallsignAsString() const { return m_callsign.asString(); }

            //! Set callsign
            void setCallsign(const CCallsign &callsign) { this->m_callsign = callsign; this->m_pilot.setCallsign(callsign); }

            //! Get situation.
            const CAircraftSituation &getSituation() const { return m_situation; }

            //! Set situation.
            void setSituation(const CAircraftSituation &situation) { m_situation = situation; }

            //! Get user
            const BlackMisc::Network::CUser &getPilot() const { return m_pilot; }

            //! Get user's real name
            QString getPilotRealname() { return m_pilot.getRealName(); }

            //! Get user's real id
            QString getPilotId() { return m_pilot.getId(); }

            //! Set user
            void setPilot(const BlackMisc::Network::CUser &user) { m_pilot = user; this->m_pilot.setCallsign(this->m_callsign);}

            //! Get ICAO info
            const CAircraftIcao &getIcaoInfo() const { return m_icao; }

            //! Set ICAO info
            void setIcaoInfo(const CAircraftIcao &icao) { m_icao = icao; }

            //! Get the distance to own plane
            const BlackMisc::PhysicalQuantities::CLength &getDistanceToPlane() const { return m_distanceToPlane; }

            //! Set distance to own plane
            void setDistanceToPlane(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_distanceToPlane = distance; }

            //! Valid distance?
            bool hasValidDistance() const { return !this->m_distanceToPlane.isNegativeWithEpsilonConsidered();}

            //! Has valid realname?
            bool hasValidRealName() const { return this->m_pilot.hasValidRealName(); }

            //! Has valid id?
            bool hasValidId() const { return this->m_pilot.hasValidId(); }

            //! Valid designators?
            bool hasValidAircraftAndAirlineDesignator() const { return this->m_icao.hasAircraftAndAirlineDesignator(); }

            //! Distance to aircraft
            PhysicalQuantities::CLength calculcateDistanceToPosition(const Geo::CCoordinateGeodetic &position) const;

            /*!
             * Calculcate distance to plane, set it, and also return it
             * \param position calculated from this postion to my own aircraft
             */
            const BlackMisc::PhysicalQuantities::CLength &setCalculcatedDistanceToPosition(const BlackMisc::Geo::CCoordinateGeodetic &position);

            //! Get position
            BlackMisc::Geo::CCoordinateGeodetic getPosition() const { return this->m_situation.getPosition(); }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_situation.setPosition(position); }

            //! Get altitude
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_situation.getAltitude(); }

            //! Set altitude
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_situation.setAltitude(altitude); }

            //! Get groundspeed
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_situation.getGroundSpeed(); }

            //! \copydoc ICoordinateGeodetic::latitude
            virtual const BlackMisc::Geo::CLatitude &latitude() const override { return this->m_situation.latitude(); }

            //! \copydoc ICoordinateGeodetic::longitude
            virtual const BlackMisc::Geo::CLongitude &longitude() const override { return this->m_situation.longitude(); }

            //! \copydoc CCoordinateGeodetic::height
            const BlackMisc::PhysicalQuantities::CLength &getHeight() const { return this->m_situation.getHeight(); }

            //! Get heading
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_situation.getHeading(); }

            //! Get pitch
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_situation.getPitch(); }

            //! Get bank
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_situation.getBank(); }

            //! Get COM1 system
            const CComSystem &getCom1System() const { return this->m_com1system; }

            //! Get COM2 system
            const CComSystem &getCom2System() const { return this->m_com2system; }

            //! Set COM1 system
            void setCom1System(const CComSystem &comSystem) { this->m_com1system = comSystem; }

            //! Set COM2 system
            void setCom2System(const CComSystem &comSystem) { this->m_com2system = comSystem; }

            //! Given SELCAL selected?
            bool isSelcalSelected(const BlackMisc::Aviation::CSelcal &selcal) const { return this->m_selcal == selcal; }

            //! Valid SELCAL?
            bool hasValidSelcal() const { return this->m_selcal.isValid(); }

            //! SELCAL
            const CSelcal getSelcal() const { return m_selcal; }

            //! Cockpit data
            void setCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder);

            //! Cockpit data
            void setCockpit(const CComSystem &com1, const CComSystem &com2, qint32 transponderCode);

            //! Own SELCAL code
            void setSelcal(const BlackMisc::Aviation::CSelcal &selcal) { this->m_selcal = selcal; }

            //! Changed cockpit data?
            bool hasChangedCockpitData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder) const;

            //! Identical COM system?
            bool hasSameComData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder);

            //! Is any (COM1/2) active frequency within 8.3383kHz channel?
            bool isActiveFrequencyWithin8_33kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return this->m_com1system.isActiveFrequencyWithin8_33kHzChannel(comFrequency) ||
                       this->m_com2system.isActiveFrequencyWithin8_33kHzChannel(comFrequency);
            }

            //! Is any (COM1/2) active frequency within 25kHz channel?
            bool isActiveFrequencyWithin25kHzChannel(const BlackMisc::PhysicalQuantities::CFrequency &comFrequency) const
            {
                return this->m_com1system.isActiveFrequencyWithin25kHzChannel(comFrequency) ||
                       this->m_com2system.isActiveFrequencyWithin25kHzChannel(comFrequency);
            }

            //! Get transponder
            const BlackMisc::Aviation::CTransponder &getTransponder() const { return this->m_transponder; }

            //! Set transponder
            void setTransponder(const CTransponder &transponder) { this->m_transponder = transponder; }

            //! Get transponder code
            QString getTransponderCodeFormatted() const { return this->m_transponder.getTransponderCodeFormatted(); }

            //! Get transponder code
            qint32 getTransponderCode() const { return this->m_transponder.getTransponderCode(); }

            //! Get transponder mode
            BlackMisc::Aviation::CTransponder::TransponderMode getTransponderMode() const { return this->m_transponder.getTransponderMode(); }

            //! Is valid for login?
            bool isValidForLogin() const;

            //! Meaningful default settings for COM Systems
            void initComSystems();

            //! Meaningful default settings for Transponder
            void initTransponder();

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        protected:
            //! \copydoc CValueObject::convertToQString()
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraft)
            CCallsign m_callsign;
            BlackMisc::Network::CUser m_pilot;
            CAircraftSituation m_situation;
            BlackMisc::Aviation::CComSystem m_com1system;
            BlackMisc::Aviation::CComSystem m_com2system;
            BlackMisc::Aviation::CTransponder m_transponder;
            BlackMisc::Aviation::CSelcal m_selcal;
            CAircraftIcao m_icao;
            BlackMisc::PhysicalQuantities::CLength m_distanceToPlane;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraft, (o.m_callsign, o.m_pilot, o.m_situation, o.m_com1system, o.m_com2system, o.m_transponder, o.m_icao, o.m_distanceToPlane))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraft)

#endif // guard
