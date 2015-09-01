/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_H

#include "blackgui/pluginconfig.h"


namespace BlackSimPlugin
{
    namespace XPlane
    {
        /**
         * Config plugin for the X-Plane plugin.
         */
        class CSimulatorXPlaneConfig : public QObject, public BlackGui::IPluginConfig
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "org.swift-project.blackgui.pluginconfiginterface" FILE "simulator_xplane_config.json")
            Q_INTERFACES(BlackGui::IPluginConfig)

        public:
            //! Ctor
            CSimulatorXPlaneConfig(QObject *parent = nullptr);

            //! \copydoc BlackGui::IPluginConfig::createConfigWindow()
            QWidget *createConfigWindow() override;

        };
    }
}

#endif // SIMULATORXPLANECONFIG_H
