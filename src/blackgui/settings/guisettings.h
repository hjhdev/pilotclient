/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_GUI_H
#define BLACKGUI_SETTINGS_GUI_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/simulation/aircraftmodel.h"

#include <QString>
#include <QAbstractItemView>

namespace BlackGui
{
    namespace Settings
    {
        //! General GUI settings
        class BLACKGUI_EXPORT CGeneralGuiSettings :
            public BlackMisc::CValueObject<CGeneralGuiSettings>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexWidgetStyle = BlackMisc::CPropertyIndex::GlobalIndexCGeneralGuiSettings,
                IndexPreferredSelection
            };

            //! Default constructor
            CGeneralGuiSettings();

            //! Widget style
            const QString &getWidgetStyle() const { return m_widgetStyle; }

            //! Widget style
            void setWidgetStyle(const QString &widgetStyle);

            //! Has changed widget style
            bool isDifferentValidWidgetStyle(const QString &style) const;

            //! Preferred selection
            QAbstractItemView::SelectionMode getPreferredSelection() const;

            //! Preferred selection
            void setPreferredSelection(QAbstractItemView::SelectionMode selection);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            QString m_widgetStyle { "Fusion" };
            int m_preferredSelection = static_cast<int>(QAbstractItemView::ExtendedSelection);

            BLACK_METACLASS(
                CGeneralGuiSettings,
                BLACK_METAMEMBER(widgetStyle),
                BLACK_METAMEMBER(preferredSelection)
            );
        };

        //! General GUI settings
        struct TGeneralGui : public BlackMisc::TSettingTrait<CGeneralGuiSettings>
        {
            //! \copydoc BlackCore::TSettingTrait::key
            static const char *key() { return "guigeneral"; }
        };

        //! Settings for last manual entries of own aircraft mode
        struct TOwnAircraftModel : public BlackMisc::TSettingTrait<BlackMisc::Simulation::CAircraftModel>
        {
            //! Key in data cache
            static const char *key() { return "guinownaircraftmodel"; }
        };

        //! Settings for last manual entries of own aircraft mode
        struct TBackgroundConsolidation : public BlackMisc::TSettingTrait<int>
        {
            //! Key in data cache
            static const char *key() { return "backgroundconsolidation"; }

            //! Validator function.
            static bool isValid(const int &valueInSeconds) { return valueInSeconds == -1 || (valueInSeconds >= minSecs() && valueInSeconds <= maxSecs()); }

            //! Default, not consolidating
            static const int &defaultValue() { static const int i = -1; return i; }

            //! Minimum
            static int minSecs() { return 60; }

            //! Maximum
            static int maxSecs() { return 3600; }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CGeneralGuiSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CGeneralGuiSettings>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackGui::Settings::CGeneralGuiSettings>)

#endif // guard
