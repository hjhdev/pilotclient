/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENT_H

#include "blackcore/data/authenticateduser.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/menudelegate.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/components/data/lastselections.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/statusmessagelist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbMappingComponent; }

namespace BlackGui
{
    namespace Components
    {
        class CDbAutoStashingComponent;
        class CDbModelMappingModifyComponent;

        /*!
         * Mapping component
         */
        class BLACKGUI_EXPORT CDbMappingComponent :
            public BlackGui::COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackGui::CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Tab index
            //! Must match real tab index
            enum TabIndex
            {
                NoValidTab   =  -1,
                TabStash     =   0,
                TabOwnModels =   1,
                TabOwnModelSet = 2,
                TabVPilot      = 3
            };

            //! Constructor
            explicit CDbMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbMappingComponent();

            //! Graceful shutdown
            void gracefulShutdown();

            //! With vPilot rules
            bool withVPilot() const { return m_withVPilot; }

            //! Any models which can be stashed
            bool hasSelectedModelsToStash() const;

            //! Models to be stashed from currently activated tab (table view)
            BlackMisc::Simulation::CAircraftModelList getSelectedModelsToStash() const;

            //! Stashed models
            const BlackMisc::Simulation::CAircraftModelList &getStashedModels() const;

            //! Stashed model strings
            QStringList getStashedModelStrings() const;

            //! Own (installed) model for given model string
            BlackMisc::Simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models
            BlackMisc::Simulation::CAircraftModelList getOwnModels() const;

            //! Own models for simulator
            const BlackMisc::Simulation::CSimulatorInfo &getOwnModelsSimulator() const;

            //! Number of own models
            int getOwnModelsCount() const;

            //! Current tab index
            TabIndex currentTabIndex() const;

            //! Current model view
            BlackGui::Views::CAircraftModelView *currentModelView() const;

            //! Is stashed view
            bool isStashedTab() const;

            //! Unvalidated consolidated aircraft model from the editor subparts (icao, distributor)
            //! \note not guaranteed to be valid, just a snapshot of its current editor state
            BlackMisc::Simulation::CAircraftModel getEditorAircraftModel() const;

        public slots:
            //! \copydoc CDbStashComponent::stashModel
            BlackMisc::CStatusMessage stashModel(const BlackMisc::Simulation::CAircraftModel &model, bool replace = false);

            //! \copydoc CDbStashComponent::stashModels
            BlackMisc::CStatusMessageList stashModels(const BlackMisc::Simulation::CAircraftModelList &models);

            //! \copydoc CDbStashComponent::consolidateModel
            BlackMisc::Simulation::CAircraftModel consolidateModel(const BlackMisc::Simulation::CAircraftModel &model) const;

            //! \copydoc CDbStashComponent::replaceModelsUnvalidated
            void replaceStashedModelsUnvalidated(const BlackMisc::Simulation::CAircraftModelList &models) const;

            //! Validate, empty list means OK
            BlackMisc::CStatusMessageList validateCurrentModel(bool withNestedForms) const;

            //! Resize so that selection is easy (larger table view)
            void resizeForSelect();

            //! Resize so that mapping is easier
            void resizeForMapping();

            //! Max. space for table view
            void maxTableView();

            //! Stash models
            void stashSelectedModels();

        signals:
            //! Request to filter by livery
            void filterByLivery(const BlackMisc::Aviation::CLivery &livery);

            //! Request to filter by aircraft ICAO
            void filterByAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Request to filter by distributor
            void filterByDistributor(const BlackMisc::Simulation::CDistributor &distributor);

            //! Request latest (incremental) data from backend
            void requestUpdatedData(BlackMisc::Network::CEntityFlags::Entity entities);

            //! Tab index has been changed
            void tabIndexChanged(int index);

        private slots:
            //! Load the vPilot rules
            void ps_loadVPilotData();

            //! Data for vPilot have been loaded
            void ps_onLoadVPilotDataFinished(bool success);

            //! vPilot cached models changed
            void ps_onVPilotCacheChanged();

            //! Request update of vPilot data
            void ps_requestVPilotDataUpdate();

            //! Request own models
            void ps_requestOwnModelsUpdate();

            //! Stashed models changed
            void ps_onStashedModelsChanged();

            //! Models have been published successfully
            void ps_onModelsSuccessfullyPublished(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Stash drop request
            void ps_handleStashDropRequest(const BlackMisc::Aviation::CAirlineIcaoCode &code) const;

            //! Row count for vPilot data changed
            void ps_onVPilotCountChanged(int count, bool withFilter);

            //! Stash count has been changed
            void ps_onStashCountChanged(int count, bool withFilter);

            //! Model set count has been changed
            void ps_onModelSetCountChanged(int count, bool withFilter);

            //! Row has been selected
            void ps_onModelRowSelected(const QModelIndex &index);

            //! Load the models
            void ps_loadInstalledModels(const BlackMisc::Simulation::CSimulatorInfo &simInfo, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode);

            //! Model loading finished
            void ps_onOwnModelsLoadingFinished(bool success, const BlackMisc::Simulation::CSimulatorInfo &simInfo);

            //! Own model count changed
            void ps_onOwnModelsCountChanged(int count, bool withFilter);

            //! Request simulator models
            void ps_requestSimulatorModels(const BlackMisc::Simulation::CSimulatorInfo &simInfo, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode);

            //! User object changed
            void ps_userChanged();

            //! Stash current model
            void ps_stashCurrentModel();

            //! Display auto stashing dialog
            void ps_displayAutoStashingDialog();

            //! Remove DB models from current view
            void ps_removeDbModelsFromView();

            //! Toggle auto filtering
            void ps_toggleAutoFiltering();

            //! Apply current DB data from form
            void ps_applyDbData();

            //! Open model modify dialog
            void ps_modifyModelDialog();

            //! Open simulator file
            void ps_showSimulatorFile();

        private:
            QScopedPointer<Ui::CDbMappingComponent>                             ui;
            QScopedPointer<CDbAutoStashingComponent>                            m_autoStashDialog;          //!< dialog auto stashing
            QScopedPointer<CDbModelMappingModifyComponent>                      m_modelModifyDialog;        //!< dialog when modifying models
            BlackMisc::Simulation::FsCommon::CVPilotRulesReader                 m_vPilotReader;             //!< read vPilot rules
            std::unique_ptr<BlackMisc::Simulation::IAircraftModelLoader>        m_modelLoader;              //!< read own aircraft models
            BlackMisc::CData<BlackMisc::Simulation::Data::VPilotAircraftModels> m_cachedVPilotModels { this, &CDbMappingComponent::ps_onVPilotCacheChanged }; //!< cache for latest vPilot rules
            BlackMisc::CData<BlackCore::Data::AuthenticatedDbUser>              m_swiftDbUser {this, &CDbMappingComponent::ps_userChanged};
            BlackMisc::CData<BlackGui::Components::Data::DbMappingComponent>    m_lastInteractions {this};  //!< last interactions
            bool m_vPilot1stInit       = true;
            bool m_withVPilot          = false;
            bool m_autoFilterInDbViews = false;  //!< automatically filter the DB view by the current model

            //! Init vPilot if rights and suitable
            void initVPilotLoading();

            //! Init model loader
            bool initModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simInfo);

            //! Model for given index from sender/current view
            BlackMisc::Simulation::CAircraftModel getModelFromView(const QModelIndex &index) const;

            //! Current tab text
            QString currentTabText() const;

            // -------------------- component specific menus --------------------------

            //! The menu for loading and handling own models for mapping tasks
            //! \note This is specific for that very component
            class CMappingOwnSimulatorModelMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CMappingOwnSimulatorModelMenu(CDbMappingComponent *mappingComponent, bool separator = true) :
                    BlackGui::IMenuDelegate(mappingComponent, separator)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;
            };

            //! The menu for loading and handling VPilot rules for mapping tasks
            //! \note This is a specific menu for that very component
            class CMappingVPilotMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CMappingVPilotMenu(CDbMappingComponent *mappingComponent, bool separator = true) :
                    BlackGui::IMenuDelegate(mappingComponent, separator)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;
            };

            //! Menu for tools:
            //! -# removing DB models from current view and
            //! -# for auto stashing
            //! -# toggle auto filtering
            //! \note This is a specific menu for that very component
            class CModelStashTools : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CModelStashTools(CDbMappingComponent *mappingComponent, bool separator = true) :
                    BlackGui::IMenuDelegate(mappingComponent, separator)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;
            };

            //! Apply DB data to selected models
            class CApplyDbDataMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CApplyDbDataMenu(CDbMappingComponent *mappingComponent, bool separator = true) :
                    BlackGui::IMenuDelegate(mappingComponent, separator)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;
            };

            //! Open up the simulator file (e.g. aircraft.cfg) in the standard text editor
            class CShowSimulatorFile : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CShowSimulatorFile(CDbMappingComponent *mappingComponent, bool separator = true) :
                    BlackGui::IMenuDelegate(mappingComponent, separator)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;
            };
        };
    } // ns
} // ns
#endif // guard
