/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "testaircraftsituation.h"
#include "blackmisc/network/fsdsetup.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/math/mathutils.h"

#include <QTest>
#include <QDateTime>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;

namespace BlackMiscTest
{
    CTestAircraftSituation::CTestAircraftSituation(QObject *parent): QObject(parent)
    {
        // void
    }

    void CTestAircraftSituation::allGndFlagsAndTakeOff() const
    {
        CAircraftSituationList situations = testSituations();
        situations.setOnGround(CAircraftSituation::OnGround);
        const CAircraftSituationChange change(situations, cg(), false);
        QVERIFY2(change.isConstOnGround(), "Expect const on ground");
        QVERIFY(!change.isConstNotOnGround());
        QVERIFY(!change.isJustTakingOff());
        QVERIFY(!change.isJustTouchingDown());
        QVERIFY(change.wasConstOnGround());
        QVERIFY(!change.wasConstNotOnGround());
        QVERIFY(situations.isSortedAdjustedLatestFirstWithoutNullPositions());

        CAircraftSituation f = situations.front();
        f.setOnGround(false);
        situations.pop_front();
        situations.push_front(f);
        const CAircraftSituationChange change2(situations, cg(), false);
        QVERIFY2(change2.isJustTakingOff(), "Expect just take off");
        QVERIFY(!change2.isJustTouchingDown());
        QVERIFY(change.wasConstOnGround());
        QVERIFY(!change.wasConstNotOnGround());
    }

    void CTestAircraftSituation::allNotGndFlagsAndTouchdown() const
    {
        CAircraftSituationList situations = testSetDescendingAltitudes(testSituations());
        situations.setOnGround(CAircraftSituation::NotOnGround);
        const CAircraftSituationChange change(situations, cg(), false);
        QVERIFY2(change.isConstNotOnGround(), "Expect const not on ground");
        QVERIFY(!change.isConstOnGround());
        QVERIFY(!change.isJustTakingOff());
        QVERIFY(!change.isJustTouchingDown());
        QVERIFY(!change.wasConstOnGround());
        QVERIFY(change.wasConstNotOnGround());
        QVERIFY(situations.isSortedAdjustedLatestFirstWithoutNullPositions());

        CAircraftSituation f = situations.front();
        f.setOnGround(true);
        situations.pop_front();
        situations.push_front(f);
        const CAircraftSituationChange change2(situations, cg(), false);
        QVERIFY2(change2.isJustTouchingDown(), "Expect just touchdown");
        QVERIFY(!change2.isJustTakingOff());
        QVERIFY(!change.wasConstOnGround());
        QVERIFY(change.wasConstNotOnGround());
    }

    void CTestAircraftSituation::ascending()
    {
        const CAircraftSituationList situations = testSituations();
        QVERIFY2(situations.isConstAscending(), "Expect ascending");
    }

    void CTestAircraftSituation::descending()
    {
        const CAircraftSituationList situations = testSetDescendingAltitudes(testSituations());
        QVERIFY2(situations.isConstDescending(), "Expect descending");
    }

    void CTestAircraftSituation::rotateUp()
    {
        CAircraftSituationList situations = testSetRotateUpPitch(testSituations());
        const CAircraftSituationChange change(situations, cg(), false);
        QVERIFY2(!change.isRotatingUp(), "Do not expect rotate up");

        CAircraftSituation f = situations.front();
        situations.pop_front();
        f.setPitch(CAngle(7.3, CAngleUnit::deg()));
        situations.push_front(f);

        const CAircraftSituationChange change2(situations, cg(), false);
        QVERIFY2(change2.isRotatingUp(), "Expect rotate up");
    }

    void CTestAircraftSituation::sortOrder() const
    {
        CAircraftSituationList situations = testSituations();
        QVERIFY2(situations.isSortedAdjustedLatestFirstWithoutNullPositions(), "Expect latest first");
        QVERIFY(!situations.isSortedLatestLast());

        situations.reverse();
        QVERIFY2(situations.isSortedAdjustedLatestLast(), "Expect latest first");
        QVERIFY(!situations.isSortedAdjustedLatestFirst());
    }

    void CTestAircraftSituation::altitudeCorrection()
    {
        CAircraftSituation::AltitudeCorrection correction = CAircraftSituation::NoCorrection;
        const CAircraftSituationList situations = testSituations();
        CAircraftSituation situation = situations.front();
        CAltitude alt(100, CAltitude::MeanSeaLevel, CLengthUnit::ft());

        situation.setAltitude(alt);
        CAltitude corAlt = situation.getCorrectedAltitude(true, &correction);

        // no elevation, expect same values
        QVERIFY2(corAlt == alt, "Expect same altitude");

        CElevationPlane ep(situation, CElevationPlane::singlePointRadius());
        situation.setGroundElevation(ep);

        // now we have same alt and elevation values
        // no elevation, expect same values
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(corAlt == alt, "Still expect same altitude");

        // now we use CG underflow detection
        const CLength cg(3, CLengthUnit::m());
        situation.setCG(cg);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::Underflow, "Expect underflow correction");
        QVERIFY2(corAlt > alt, "Expect corrected altitude");
        QVERIFY2((corAlt - cg) == alt, "Expect correction by CG");

        // Unit of CG must be irrelevant
        CLength cg2(cg);
        cg2.switchUnit(CLengthUnit::m());
        situation.setCG(cg2);
        CAltitude corAlt2 = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::Underflow, "Expect underflow correction");
        QVERIFY2(corAlt2 == corAlt, "Expect same value for corrected altitude");

        // now we test a negative altitude
        alt = CAltitude(-1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
        ep.setGeodeticHeight(alt);
        situation.setAltitude(alt);
        situation.setGroundElevation(ep);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::Underflow, "Expect underflow correction");
        QVERIFY2(corAlt > alt, "Expect corrected altitude");
        QVERIFY2((corAlt - cg) == alt, "Expect correction by CG");

        // overflow detection
        alt = CAltitude(1000, CAltitude::MeanSeaLevel, CLengthUnit::ft());
        situation.setAltitude(alt);
        ep.setGeodeticHeight(alt);
        ep.addAltitudeOffset(CLength(-100, CLengthUnit::ft()));
        situation.setGroundElevation(ep);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(corAlt == alt, "Expect same altitude, no overflow since not on gnd.");

        situation.setOnGround(CAircraftSituation::OnGround, CAircraftSituation::InFromNetwork);
        corAlt = situation.getCorrectedAltitude(true, &correction);
        QVERIFY2(correction == CAircraftSituation::DraggedToGround, "Expect dragged to gnd.");
        QVERIFY2(corAlt < alt, "Expect corrected altitude dragged to gnd.");
        QVERIFY2(corAlt == (ep.getAltitude() + cg), "Expect correction by CG");
    }

    CAircraftSituationList CTestAircraftSituation::testSituations()
    {
        // "Kugaaruk Airport","Pelly Bay","Canada","YBB","CYBB",68.534401,-89.808098,56,-7,"A","America/Edmonton","airport","OurAirports"
        // "Baie Comeau Airport","Baie Comeau","Canada","YBC","CYBC",49.13249969482422,-68.20439910888672,71,-5,"A","America/Toronto","airport","OurAirports"
        // "CFB Bagotville","Bagotville","Canada","YBG","CYBG",48.33060073852539,-70.99639892578125,522,-5,"A","America/Toronto","airport","OurAirports"

        CAircraftSituationList situations;
        qint64 ts = QDateTime::currentSecsSinceEpoch();
        qint64 os = CFsdSetup::c_positionTimeOffsetMsec;
        CAltitude alt(10000, CAltitude::MeanSeaLevel, CLengthUnit::m());
        static const CCoordinateGeodetic dummyPos(48.33060073852539, -70.99639892578125, 522);

        for (int i = 0; i < 10; i++)
        {
            CAircraftSituation s(dummyPos);
            const qint64 cTs = ts - i * os;
            s.setMSecsSinceEpoch(cTs);
            s.setTimeOffsetMs(os);
            CAltitude altitude(alt);
            altitude.addValueSameUnit(-100 * i); // 10000, 9900, 9800 .... (newer->older)
            s.setAltitude(altitude);
            situations.push_back(s);
        }
        return situations;
    }

    CAircraftSituationList CTestAircraftSituation::testSetDescendingAltitudes(const CAircraftSituationList &situations)
    {
        CAircraftSituationList newSituations;
        CAltitude alt(0, CAltitude::MeanSeaLevel, CLengthUnit::m());

        for (const CAircraftSituation &situation : situations)
        {
            CAircraftSituation s(situation);
            s.setAltitude(alt);
            newSituations.push_back(s);
            alt.addValueSameUnit(100); // 0, 100, 200 ... (newer->older)
        }
        return newSituations;
    }

    CAircraftSituationList CTestAircraftSituation::testSetRotateUpPitch(const CAircraftSituationList &situations)
    {
        CAircraftSituationList newSituations;
        double average = 0;
        for (const CAircraftSituation &situation : situations)
        {
            CAircraftSituation s(situation);
            const double pitch = CMathUtils::randomDouble(1.5);
            average += pitch;
            s.setPitch(CAngle(pitch, CAngleUnit::deg()));
            newSituations.push_back(s);
        }
        average = average / newSituations.size();
        CAircraftSituation avg = newSituations.front();
        avg.setPitch(CAngle(average, CAngleUnit::deg()));
        avg.addMsecs(1000); // make this the latest situation
        newSituations.push_front(avg); // first value is average pitch, so it will NOT be detected
        return newSituations;
    }

    const CLength &CTestAircraftSituation::cg()
    {
        static const CLength cg(2.0, CLengthUnit::m());
        return cg;
    }
} // namespace

//! \endcond
