/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "statusmessageform.h"
#include "ui_statusmessageform.h"
#include <QLabel>

namespace BlackGui
{
    CStatusMessageForm::CStatusMessageForm(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CStatusMessageForm)
    {
        ui->setupUi(this);
    }

    CStatusMessageForm::~CStatusMessageForm()
    { }

    void CStatusMessageForm::setValue(const BlackMisc::CStatusMessage &message)
    {
        if (!this->isVisible()) { return; }
        ui->te_Message->setPlainText(message.getMessage());
        ui->lbl_SeverityIcon->setPixmap(message.toPixmap());
        ui->le_Categories->setText(message.getCategories().toQString(true));
        ui->le_Severity->setText(message.getSeverityAsString());
        ui->le_Timestamp->setText(message.getFormattedUtcTimestampYmdhms());
    }

    void CStatusMessageForm::toggleVisibility()
    {
        this->setVisible(!isVisible());
    }

} // ns
