/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_MAPPINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_MAPPINGCOMPONENT_H

#include "blackcore/network.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackmisc/identifier.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <Qt>

class QCompleter;
class QModelIndex;
class QWidget;

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}
namespace Ui { class CMappingComponent; }

namespace BlackGui
{
    namespace Views { class CCheckBoxDelegate; }

    namespace Components
    {
        class CUpdateTimer;

        //! Mappings, models etc.
        class BLACKGUI_EXPORT CMappingComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CMappingComponent();

            //! Number of current mappings
            int countCurrentMappings() const;

            //! Numer of models
            int countAircraftModels() const;

            //! Find models starting with
            BlackMisc::Simulation::CAircraftModelList findModelsStartingWith(const QString modelName, Qt::CaseSensitivity cs);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        private slots:
            //! Aircraft models available
            void ps_onAircraftModelsLoaded();

            //! Model matched
            void ps_onModelMatchingCompleted(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Changed count
            void ps_onRowCountChanged(int count, bool withFilter);

            //! Simulated aircraft did change in view
            //! \sa CViewBaseNonTemplate::objectChanged
            void ps_onChangedSimulatedAircraftInView(const BlackMisc::CVariant &simulatedAircraft, const BlackMisc::CPropertyIndex &index);

            //! Aircraft selected (in view)
            void ps_onAircraftSelectedInView(const QModelIndex &index);

            //! Model selected (in view)
            void ps_onModelSelectedInView(const QModelIndex &index);

            //! Save changed aircraft
            void ps_onSaveAircraft();

            //! Model preview
            void ps_onModelPreviewChanged(int state);

            //! Request update for mappings from backend
            void ps_onSimulatedAircraftUpdateRequested();

            //! Request update for models from backend
            void ps_onModelsUpdateRequested();

            //! Rendered aircraft changed in backend
            void ps_onRemoteAircraftModelChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Aircraft enabled, disabled in backend
            void ps_onChangedAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Fast position updates on/off in backend
            void ps_onFastPositionUpdatesEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Connection status has been changed
            void ps_onConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Simulator has handled airspace snapshot
            void ps_onAirspaceSnapshotHandled();

            //! Fast position updates onf/off
            void ps_onMenuChangeFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Enable / disable aircraft
            void ps_onMenuEnableAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Reload models
            void ps_onMenuRequestModelReload();

            //! Highlight in simulator
            void ps_onMenuHighlightInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        private:
            //! Identifier for data send from this component
            BlackMisc::CIdentifier mappingIdentifier();

            //! Update simulated aircraft view
            void updateSimulatedAircraftView(bool forceUpdate = false);

            QScopedPointer<Ui::CMappingComponent> ui;
            QScopedPointer<CUpdateTimer>          m_updateTimer;
            QCompleter                           *m_modelCompleter = nullptr;
            bool                                  m_missedSimulatedAircraftUpdate = true;
            BlackGui::Views::CCheckBoxDelegate   *m_currentMappingsViewDelegate = nullptr;
            BlackMisc::CIdentifier                m_identifier;

        private slots:
            //! Updated by timer
            void ps_backgroundUpdate();
        };

    } // namespace
} // namespace

#endif // guard
