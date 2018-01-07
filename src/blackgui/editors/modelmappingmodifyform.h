/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_MODELMAPPINGMODIFYFORM_H
#define BLACKGUI_EDITORS_MODELMAPPINGMODIFYFORM_H

#include "form.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/propertyindexvariantmap.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QCheckBox;
class QWidget;

namespace Ui { class CModelMappingModifyForm; }
namespace BlackMisc { namespace Simulation { class CAircraftModel; } }
namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Allows to modify individual fields of the model form
         */
        class BLACKGUI_EXPORT CModelMappingModifyForm : public CFormDbUser
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CModelMappingModifyForm(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CModelMappingModifyForm();

            //! Get the values
            BlackMisc::CPropertyIndexVariantMap getValues() const;

            //! Set value
            void setValue(const BlackMisc::Simulation::CAircraftModel &model);

            //! \copydoc CForm::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

        protected:
            //! \copydoc CFormDbUser::userChanged
            virtual void userChanged() override;

        private:
            //! Return pressed
            void returnPressed();

            //! Data changed
            void changed();

            //! Checkbox from its corresponding widgte
            QCheckBox *widgetToCheckbox(QObject *widget) const;

            QScopedPointer<Ui::CModelMappingModifyForm> ui;
        };
    } // ns
} // ns

#endif // guard
