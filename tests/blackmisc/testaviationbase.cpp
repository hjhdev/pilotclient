#include "testaviationbase.h"

namespace BlackMiscTest {

/**
 * Constructor
 */
CTestAviationBase::CTestAviationBase(QObject *parent): QObject(parent)
{
    // void
}

/**
 * Basic tests
 */
void CTestAviationBase::headingBasics()
{
    CHeading h1(180, true, CAngleUnit::deg());
    CHeading h2(180, false, CAngleUnit::deg());
    CHeading h3(181, true, CAngleUnit::deg());
    CAngle a1(200,CAngleUnit::deg());
    CHeading h4;
    h4 = h1;
    QVERIFY2(h1 != h2, "Magnetic and true heading are not the same");
    QVERIFY2(h1 < h3, "180deg are less than 181deg");
    QVERIFY2(a1 > h3, "200deg are more than 200deg");
    QVERIFY2(h4 == h1, "Values shall be equal");

    h1 -= h1;
    QVERIFY2(h1.unitValueToDouble() == 0, "Value shall be 0");

    // h4 = h1 + h2; does not work, because misleading
    h2 += h2; // add just angle
    QVERIFY2(h2.unitValueToDouble() == 360, "Value shall be 360");
}

/**
 * Vertical positions
 */
void CTestAviationBase::verticalPosition()
{
    CAviationVerticalPositions vp1 = CAviationVerticalPositions::fromAltitudeAndElevationInFt(10000.0, 3000.0);
    CAviationVerticalPositions vp2 = vp1;
    QVERIFY2(vp1== vp2, "Values shall be equal");
}

} // namespace
