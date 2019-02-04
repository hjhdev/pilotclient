/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_STATUSMESSAGEVIEW_H
#define BLACKGUI_VIEWS_STATUSMESSAGEVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/statusmessagelistmodel.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/statusmessage.h"
#include "blackgui/blackguiexport.h"

#include <QObject>

namespace BlackGui
{
    namespace Filters { class CStatusMessageFilterDialog; }
    namespace Views
    {
        //! Status message view
        class BLACKGUI_EXPORT CStatusMessageView : public CViewBase<Models::CStatusMessageListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CStatusMessageView(QWidget *parent = nullptr);

            //! Set mode
            void setMode(Models::CStatusMessageListModel::Mode mode);

            //! Add my own filter dialog
            void addFilterDialog();

            //! \copydoc BlackMisc::CStatusMessageList::keepLatest
            void keepLatest(int desiredSize);

            //! Filter dialog if any
            Filters::CStatusMessageFilterDialog *getFilterDialog() const;
        };
    } // ns
} // ns
#endif // guard
