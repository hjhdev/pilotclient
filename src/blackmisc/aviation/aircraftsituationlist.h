/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATIONLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATIONLIST_H

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QList>
#include <QPair>
#include <QHash>

namespace BlackMisc
{
    namespace Geo { class CElevationPlane; }
    namespace Simulation { class CAircraftModel; }
    namespace Aviation
    {
        class CAircraftParts;

        //! List of aircraft situations
        class BLACKMISC_EXPORT CAircraftSituationList :
            public CSequence<CAircraftSituation>,
            public Geo::IGeoObjectList<CAircraftSituation, CAircraftSituationList>,
            public ITimestampWithOffsetObjectList<CAircraftSituation, CAircraftSituationList>,
            public ICallsignObjectList<CAircraftSituation, CAircraftSituationList>,
            public Mixin::MetaType<CAircraftSituationList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftSituationList)

            //! Default constructor.
            CAircraftSituationList();

            //! Construct from a base class object.
            CAircraftSituationList(const CSequence<CAircraftSituation> &other);

            //! Construct from initializer list.
            CAircraftSituationList(std::initializer_list<CAircraftSituation> il);

            //! Set ground elevation from elevation plane
            int setGroundElevationChecked(const Geo::CElevationPlane &elevationPlane, CAircraftSituation::GndElevationInfo info, qint64 newerThanAdjustedMs = -1);

            //! Set ground elevation from elevation plane and guess ground
            //! \note requires a sorted list latest first
            int setGroundElevationCheckedAndGuessGround(const Geo::CElevationPlane &elevationPlane, CAircraftSituation::GndElevationInfo info, const Simulation::CAircraftModel &model, CAircraftSituationChange *changeOut);

            //! Adjust flag from parts by using CAircraftSituation::adjustGroundFlag
            int adjustGroundFlag(const CAircraftParts &parts, double timeDeviationFactor = 0.1);

            //! Extrapolate ground flag into the future
            int extrapolateGroundFlag();

            //! Find if having inbound information
            CAircraftSituationList findByInboundGroundInformation(bool hasGroundInfo) const;

            //! Any situation without ground info?
            bool containsSituationWithoutGroundElevation() const;

            //! Any situation outside range?
            bool containsGroundElevationOutsideRange(const PhysicalQuantities::CLength &range) const;

            //! Contains on ground details?
            bool containsOnGroundDetails(CAircraftSituation::OnGroundDetails details) const;

            //! Contains any push back?
            //! \remark only valid for non VTOL aircraft
            bool containsPushBack() const;

            //! Contains any gnd.flag \c true ?
            bool containsOnGroundFromNetwork() const;

            //! Are all on ground details the same?
            bool areAllOnGroundDetailsSame(CAircraftSituation::OnGroundDetails details) const;

            //! Are all situations on ground?
            bool isConstOnGround() const;

            //! Are all situations not on ground?
            bool isConstNotOnGround() const;

            //! Constantly descending?
            bool isConstDescending(bool alreadySortedLatestFirst = false) const;

            //! Constantly ascending?
            bool isConstAscending(bool alreadySortedLatestFirst = false) const;

            //! Constantly accelerating?
            bool isConstAccelerating(bool alreadySortedLatestFirst = false) const;

            //! Constantly decelarating?
            bool isConstDecelarating(bool alreadySortedLatestFirst = false) const;

            //! Is the ground flag changing for the recent situations
            bool isGndFlagChanging(bool alreadySortedLatestFirst = false) const;

            //! Is just taking off?
            bool isJustTakingOff(bool alreadySortedLatestFirst = false) const;

            //! Is just touch down?
            bool isJustTouchingDown(bool alreadySortedLatestFirst = false) const;

            //! Is rotating up?
            bool isRotatingUp(bool alreadySortedLatestFirst = false) const;

            //! Count the number of situations with CAircraftSituation::IsOnGround
            int countOnGround(CAircraftSituation::IsOnGround og) const;

            //! Count the number of situations with CAircraftSituation::IsOnGround and elevation
            int countOnGroundWithElevation(CAircraftSituation::IsOnGround og) const;

            //! Situations with CAircraftSituation::IsOnGround and elevation
            CAircraftSituationList findOnGroundWithElevation(CAircraftSituation::IsOnGround og) const;

            //! Set on ground
            int setOnGround(CAircraftSituation::IsOnGround og);

            //! Set on ground details for all situations
            int setOnGroundDetails(CAircraftSituation::OnGroundDetails details);

            //! Add an offset to each altitude
            int addAltitudeOffset(const PhysicalQuantities::CLength &offset);

            //! Latest first and no null positions?
            bool isSortedAdjustedLatestFirstWithoutNullPositions() const;

            //! Remove the first situation
            //! \remark normally used when the first situation represents the latest situation
            CAircraftSituationList withoutFrontSituation() const;

            //! All pitch values
            QList<double> pitchValues(const PhysicalQuantities::CAngleUnit &unit) const;

            //! All ground speed values
            QList<double> groundSpeedValues(const PhysicalQuantities::CSpeedUnit &unit) const;

            //! All elevation values
            QList<double> elevationValues(const PhysicalQuantities::CLengthUnit &unit) const;

            //! All altitude values
            QList<double> altitudeValues(const PhysicalQuantities::CLengthUnit &unit) const;

            //! All corrected altitude values
            QList<double> correctedAltitudeValues(const PhysicalQuantities::CLengthUnit &unit, const PhysicalQuantities::CLength &cg) const;

            //! All ground distance values
            QList<double> groundDistanceValues(const PhysicalQuantities::CLengthUnit &unit, const PhysicalQuantities::CLength &cg) const;

            //! Pitch angles standard deviation and mean
            QPair<PhysicalQuantities::CAngle, PhysicalQuantities::CAngle> pitchStandardDeviationAndMean() const;

            //! Ground speed standard deviation and mean
            QPair<PhysicalQuantities::CSpeed, PhysicalQuantities::CSpeed> groundSpeedStandardDeviationAndMean() const;

            //! Elevation standard deviation and mean
            QPair<CAltitude, CAltitude> elevationStandardDeviationAndMean() const;

            //! Elevation standard deviation and mean
            QPair<CAltitude, CAltitude> altitudeStandardDeviationAndMean() const;

            //! Min. and max. ground distance
            QPair<PhysicalQuantities::CLength, PhysicalQuantities::CLength> minMaxGroundDistance(const PhysicalQuantities::CLength &cg) const;

            //! Elevation standard deviation and mean
            //! \note distance is without CG, so on ground it can also be used to calculate
            QPair<CAltitude, CAltitude> altitudeAglStandardDeviationAndMean() const;
        };

        //! Situation per callsign
        using CAircraftSituationPerCallsign = QHash<CCallsign, CAircraftSituation>;

        //! Situations (list) per callsign
        using CAircraftSituationListPerCallsign = QHash<CCallsign, CAircraftSituationList>;

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftSituation>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftSituation>)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationPerCallsign)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationListPerCallsign)

#endif // guard
