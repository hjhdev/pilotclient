/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATOR_H
#define BLACKMISC_SIMULATION_INTERPOLATOR_H

#include "interpolationrenderingsetup.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"

#include <QObject>
#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation
    {
        class CInterpolationHints;

        //! Interpolator, calculation inbetween positions
        class BLACKMISC_EXPORT IInterpolator :
            public QObject,
            public BlackMisc::Simulation::CRemoteAircraftAware
        {
            Q_OBJECT

        public:
            //! Virtual destructor
            virtual ~IInterpolator() {}

            //! Log category
            static QString getLogCategory() { return "swift.interpolator"; }

            //! Status of interpolation
            struct BLACKMISC_EXPORT InterpolationStatus // does not link without export/allTrue, reset
            {
            public:
                //! Did interpolation succeed?
                bool didInterpolationSucceed() const { return m_interpolationSucceeded; }

                //! Set succeeded
                void setInterpolationSucceeded(bool succeeded) { m_interpolationSucceeded = succeeded; }

                //! Changed position?
                bool hasChangedPosition() const { return m_changedPosition; }

                //! Set as changed
                void setChangedPosition(bool changed) { m_changedPosition = changed; }

                //! all OK
                bool allTrue() const;

                //! Reset to default values
                void reset();

            private:
                bool m_changedPosition = false;        //!< position was changed
                bool m_interpolationSucceeded = false; //!< interpolation succeeded (means enough values, etc.)
            };

            //! Status regarding parts
            struct BLACKMISC_EXPORT PartsStatus // does not link without export/allTrue, resetx
            {
            public:
                //! all OK
                bool allTrue() const;

                //! Supporting parts
                bool isSupportingParts() const { return m_supportsParts; }

                //! Set support flag
                void setSupportsParts(bool supports) { m_supportsParts = supports; }

                //! Reset to default values
                void reset();

            private:
                bool m_supportsParts = false;   //!< supports parts for given callsign
            };

            //! Current interpolated situation
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc,
                const CInterpolationHints &hints, InterpolationStatus &status) const;

            //! Current interpolated situation, to be implemented by subclass
            //! \threadsafe
            //! \remark public only for XP driver
            virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                const BlackMisc::Aviation::CAircraftSituationList &situations, qint64 currentTimeSinceEpoc,
                const CInterpolationHints &hints, InterpolationStatus &status) const = 0;

            //! Parts before given offset time (aka pending parts)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftParts getInterpolatedParts(
                const BlackMisc::Aviation::CAircraftPartsList &parts, qint64 cutoffTime,
                PartsStatus &partsStatus) const;

            //! Parts before given offset time (aka pending parts)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftParts getInterpolatedParts(
                const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTime,
                PartsStatus &partsStatus) const;

            //! Enable debug messages etc.
            //! \threadsafe
            void setInterpolatorSetup(const CInterpolationAndRenderingSetup &setup);

        protected:
            //! Constructor
            IInterpolator(BlackMisc::Simulation::IRemoteAircraftProvider *provider, const QString &objectName, QObject *parent);

            //! Enable debug messages etc.
            //! \threadsafe
            CInterpolationAndRenderingSetup getInterpolatorSetup() const;

            //! Set the ground elevation from hints, if possible and not already set
            static void setGroundElevationFromHint(const CInterpolationHints &hints, BlackMisc::Aviation::CAircraftSituation &situation);

            CInterpolationAndRenderingSetup m_setup; //!< allows to disable debug messages
            mutable QReadWriteLock m_lock; //!< lock interpolator
        };
    } // namespace
} // namespace
#endif // guard
