/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H
#define BLACKGUI_COMPONENTS_NAVIGATORDIALOG_H

#include "blackgui/blackguiexport.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/settings/navigatorsettings.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>
#include <QWidgetAction>
#include <QGridLayout>

class QEvent;
class QMenu;
class QMouseEvent;
class QPoint;
class QWidget;

namespace Ui { class CNavigatorDialog; }
namespace BlackGui
{
    namespace Components
    {
        class CMarginsInput;

        /*!
         * Navigator dialog
         */
        class BLACKGUI_EXPORT CNavigatorDialog :
            public QDialog,
            public CEnableForFramelessWindow
        {
            Q_OBJECT

        public:
            //! Constructor
            CNavigatorDialog(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CNavigatorDialog();

            //! Navigator
            void buildNavigator(int columns);

            //! Called when dialog is closed
            virtual void reject() override;

        signals:
            //! Navigator closed
            void navigatorClosed();

        public slots:
            //! Toggle frameless mode
            void toggleFrameless();

            //! Toggle visibility
            void toggleNavigator();

            //! Restore from settings
            void restoreFromSettings();

            //! Save to settings
            void saveToSettings();

        protected:
            //! Style sheet has changed
            void ps_onStyleSheetsChanged();

            //! \name Base class events
            //! @{
            virtual void mouseMoveEvent(QMouseEvent *event) override;
            virtual void mousePressEvent(QMouseEvent *event) override;
            virtual void changeEvent(QEvent *evt) override;
            virtual void windowFlagsChanged() override;
            virtual void paintEvent(QPaintEvent *event) override;
            //! @}

        private slots:
            //! Context menu
            void ps_showContextMenu(const QPoint &pos);

            //! Change the layout
            void ps_changeLayout();

            //! Margins context menu
            void ps_menuChangeMargins(const QMargins &margins);

            //! Dummy slot
            void ps_dummy();

            //! Changed settigs
            void ps_settingsChanged();

        private:
            //! Insert own actions
            void insertOwnActions();

            //! Contribute to menu
            void addToContextMenu(QMenu *contextMenu) const;

            //! How many columns for given rows
            int columnsForRows(int rows);

            //! Get my own grid layout
            QGridLayout *myGridLayout() const;

            //! Adjust navigator size
            void adjustNavigatorSize(QGridLayout *layout = nullptr);

            QScopedPointer<Ui::CNavigatorDialog> ui;
            bool m_firstBuild = true;
            int  m_currentColumns = 1;
            QWidgetAction *m_marginMenuAction = nullptr; //!< menu widget(!) action for margin widget
            CMarginsInput *m_input = nullptr; //!< margins widget
            BlackMisc::CSetting<BlackGui::Settings::TNavigator> m_settings { this, &CNavigatorDialog::ps_settingsChanged };
        };
    } // ns
} // ns

#endif // guard
