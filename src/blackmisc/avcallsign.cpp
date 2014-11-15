/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avcallsign.h"
#include "iconlist.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CCallsign::convertToQString(bool /** i18n **/) const
        {
            return this->m_callsign;
        }

        /*
         * Unify the callsign
         */
        QString CCallsign::unifyCallsign(const QString &callsign)
        {
            QString unified = callsign.toUpper();
            unified = unified.remove(QRegExp("[^a-zA-Z\\d\\s]"));
            return unified;
        }

        /*
         * Iconify
         */
        const CIcon &CCallsign::convertToIcon(const CCallsign &callsign)
        {
            QString t = callsign.asString().toUpper();
            if (t.length() < 3) return CIconList::iconByIndex(CIcons::NetworkRoleUnknown);
            t = t.right(3);

            if (callsign.getStringAsSet().contains("_"))
            {
                if ("APP" == t) return CIconList::iconByIndex(CIcons::NetworkRoleApproach);
                if ("GND" == t) return CIconList::iconByIndex(CIcons::NetworkRoleGround);
                if ("TWR" == t) return CIconList::iconByIndex(CIcons::NetworkRoleTower);
                if ("DEL" == t) return CIconList::iconByIndex(CIcons::NetworkRoleDelivery);
                if ("CTR" == t) return CIconList::iconByIndex(CIcons::NetworkRoleCenter);
                if ("SUP" == t) return CIconList::iconByIndex(CIcons::NetworkRoleSup);
                if ("OBS" == t) return CIconList::iconByIndex(CIcons::NetworkRoleApproach);
                return CIconList::iconByIndex(CIcons::NetworkRoleUnknown);
            }
            else
            {
                return CIconList::iconByIndex(CIcons::NetworkRolePilot);
            }
        }

        /*
         * Callsign as Observer
         */
        QString CCallsign::getAsObserverCallsignString() const
        {
            if (this->isEmpty()) return "";
            QString obs = this->getStringAsSet();
            if (obs.endsWith("_OBS", Qt::CaseInsensitive)) return obs;
            if (obs.contains('_')) obs = obs.left(obs.lastIndexOf('_'));
            return obs.append("_OBS").toUpper();
        }

        /*
         * Equals callsign?
         */
        bool CCallsign::equalsString(const QString &callsignString) const
        {
            CCallsign other(callsignString);
            return other == (*this);
        }

        /*
         * Index
         */
        QVariant CCallsign::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:
                return QVariant(this->asString());
            case IndexCallsignStringAsSet:
                return QVariant(this->getStringAsSet());
            case IndexTelephonyDesignator:
                return QVariant(this->getTelephonyDesignator());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Index
         */
        void CCallsign::setPropertyByIndex(const QVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:
                this->m_callsign = variant.toString();
                break;
            case IndexCallsignStringAsSet:
                this->m_callsignAsSet = variant.toString();
                break;
            case IndexTelephonyDesignator:
                this->m_telephonyDesignator = variant.toString();
                break;
            default:
                return CValueObject::setPropertyByIndex(variant, index);
            }
        }

        /*
         * Valid callsign?
         */
        bool CCallsign::isValidCallsign(const QString &callsign)
        {
            static QRegularExpression regexp("^[A-Z]+[A-Z0-9]*$");
            if (callsign.length() < 2 || callsign.length() > 10) return false;
            return (regexp.match(callsign).hasMatch());
        }

    } // namespace
} // namespace
