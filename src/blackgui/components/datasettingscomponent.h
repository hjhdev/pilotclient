/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_DATASETTINGSCOMPONENT_H
#define BLACKGUI_SETTINGS_DATASETTINGSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessagesframe.h"
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui { class CDataSettingsComponent; }
namespace BlackCore { namespace Db { class CBackgroundDataUpdater; }}
namespace BlackGui
{
    namespace Components
    {
        /**
         * Settings
         */
        class BLACKGUI_EXPORT CDataSettingsComponent : public BlackGui::COverlayMessagesFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDataSettingsComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDataSettingsComponent();

            //! Background updater
            void setBackgroundUpdater(BlackCore::Db::CBackgroundDataUpdater *updater);

        private:
            QScopedPointer<Ui::CDataSettingsComponent> ui;
        };
    } // ns
} // ns

#endif // guard
