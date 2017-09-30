/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_FLIGHTPLANLIST_H
#define BLACKMISC_AVIATION_FLIGHTPLANLIST_H

#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include <QMetaType>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for a list of flight plans.
        class BLACKMISC_EXPORT CFlightPlanList :
            public CSequence<CFlightPlan>,
            public ICallsignObjectList<CFlightPlan, CFlightPlanList>,
            public Mixin::MetaType<CFlightPlanList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CFlightPlanList)

            //! Default constructor.
            CFlightPlanList();

            //! Construct from a base class object.
            CFlightPlanList(const CSequence<CFlightPlan> &other);
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CFlightPlanList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CFlightPlan>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CFlightPlan>)

#endif //guard
