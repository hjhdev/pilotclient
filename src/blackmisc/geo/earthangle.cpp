/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/geo/earthangle.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/iconlist.h"

namespace BlackMisc
{
    namespace Geo
    {

        template <class LATorLON>
        CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator +=(const CEarthAngle &latOrLon)
        {
            this->PhysicalQuantities::CAngle::operator +=(latOrLon);
            return *this;
        }

        template <class LATorLON>
        CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator -=(const CEarthAngle &latOrLon)
        {
            this->PhysicalQuantities::CAngle::operator -=(latOrLon);
            return *this;
        }

        template <class LATorLON>
        CEarthAngle<LATorLON> &CEarthAngle<LATorLON>::operator *=(double multiply)
        {
            this->PhysicalQuantities::CAngle::operator *=(multiply);
            return *this;
        }

        template <class LATorLON>
        bool CEarthAngle<LATorLON>::operator >(const CEarthAngle &latOrLon) const
        {
            return this->PhysicalQuantities::CAngle::operator >(latOrLon);
        }

        template <class LATorLON>
        bool CEarthAngle<LATorLON>::operator <(const CEarthAngle &latOrLon) const
        {
            return this->PhysicalQuantities::CAngle::operator >(latOrLon);
        }

        template <class LATorLON>
        bool CEarthAngle<LATorLON>::operator <=(const CEarthAngle &latOrLon) const
        {
            return this->PhysicalQuantities::CAngle::operator <=(latOrLon);
        }

        template <class LATorLON>
        bool CEarthAngle<LATorLON>::operator >=(const CEarthAngle &latOrLon) const
        {
            return this->PhysicalQuantities::CAngle::operator >=(latOrLon);
        }

        template <class LATorLON>
        LATorLON CEarthAngle<LATorLON>::operator +(const CEarthAngle &latOrLon) const
        {
            LATorLON l(*this);
            l += latOrLon;
            return l;
        }

        template <class LATorLON>
        LATorLON CEarthAngle<LATorLON>::operator -(const CEarthAngle &latOrLon) const
        {
            LATorLON l(*this);
            l -= latOrLon;
            return l;
        }

        template <class LATorLON>
        LATorLON CEarthAngle<LATorLON>::operator *(double multiply) const
        {
            LATorLON l(*this);
            l *= multiply;
            return l;
        }

        template <class LATorLON>
        LATorLON CEarthAngle<LATorLON>::fromWgs84(const QString &wgsCoordinate)
        {
            // http://www.regular-expressions.info/floatingpoint.html
            const QString wgs = wgsCoordinate.simplified().trimmed();
            QRegExp rx("([-+]?[0-9]*\\.?[0-9]+)");
            qint32 deg = 0;
            qint32 min = 0;
            double sec = 0.0;
            double secFragment = 0.0;
            int fragmentLength = 0;
            int c = 0;
            int pos = 0;
            while ((pos = rx.indexIn(wgs, pos)) != -1)
            {
                QString cap = rx.cap(1);
                pos += rx.matchedLength();
                switch (c++)
                {
                case 0:
                    deg = cap.toInt();
                    break;
                case 1:
                    min = cap.toInt();
                    break;
                case 2:
                    sec = cap.toDouble();
                    break;
                case 3:
                    secFragment = cap.toDouble();
                    fragmentLength = cap.length();
                    break;
                default:
                    break;
                }
            }
            if (fragmentLength > 0)
            {
                // we do have given ms in string
                sec += secFragment / qPow(10, fragmentLength);
            }

            if (wgs.contains('S', Qt::CaseInsensitive) ||
                    wgs.contains('W', Qt::CaseInsensitive)) deg *= -1;

            PhysicalQuantities::CAngle a(deg, min, sec);
            return LATorLON(a);
        }

        template <class LATorLON>
        CEarthAngle<LATorLON>::CEarthAngle()
            : CEarthAngle::CValueObject(0.0, BlackMisc::PhysicalQuantities::CAngleUnit::deg())
        { }

        template <class LATorLON>
        CEarthAngle<LATorLON>::CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit)
            : CEarthAngle::CValueObject(value, unit)
        { }

        template <class LATorLON>
        CEarthAngle<LATorLON>::CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle)
            : CEarthAngle::CValueObject(angle)
        { }

        template <class LATorLON>
        QString CEarthAngle<LATorLON>::convertToQString(bool i18n) const
        {
            return this->valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CAngleUnit::deg(), 6, i18n);
        }

        template <class LATorLON>
        LATorLON const *CEarthAngle<LATorLON>::derived() const { return static_cast<LATorLON const *>(this); }

        template <class LATorLON>
        LATorLON *CEarthAngle<LATorLON>::derived() { return static_cast<LATorLON *>(this); }

        template <class LATorLON>
        CIcon CEarthAngle<LATorLON>::toIcon() const
        {
            return BlackMisc::CIconList::iconByIndex(CIcons::GeoPosition);
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CEarthAngle<CLatitude>;
        template class CEarthAngle<CLongitude>;

    } // namespace
} // namespace
