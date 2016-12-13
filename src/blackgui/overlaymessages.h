/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_OVERLAYMESSAGES_H
#define BLACKGUI_OVERLAYMESSAGES_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QFrame>
#include <QMessageBox>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTimer>
#include <functional>

class QKeyEvent;
class QPaintEvent;
class QPixmap;
class QWidget;

namespace BlackMisc { namespace Network { class CTextMessage; } }
namespace Ui { class COverlayMessages; }
namespace BlackGui
{
    /*!
     * Display status messages (nested in the parent widget)
     */
    class BLACKGUI_EXPORT COverlayMessages : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit COverlayMessages(int w, int h, QWidget *parent);

        //! Constructor
        explicit COverlayMessages(const QString &headerText, int w, int h, QWidget *parent);

        //! Destructor
        virtual ~COverlayMessages();

        //! Messages mode
        void setModeToMessages();

        //! Single Message mode
        void setModeToMessage();

        //! Single Message mode
        void setModeToMessageSmall();

        //! Single Text message mode
        void setModeToTextMessage();

        //! Display image
        void setModeToImage();

        //! Set header text
        void setHeaderText(const QString &header);

        //! Set the message and show the confirmation frame
        void setConfirmationMessage(const QString &message);

        //! Show multiple messages with confirmation bar
        void showOverlayMessagesWithConfirmation(const BlackMisc::CStatusMessageList &messages,
                const QString &confirmationMessage,
                std::function<void()> okLambda,
                int defaultButton = QMessageBox::Cancel,
                int timeOutMs = -1);

        //! Show multiple messages
        void showOverlayMessages(const BlackMisc::CStatusMessageList &messages, int timeOutMs = -1);

        //! Show single message
        void showOverlayMessage(const BlackMisc::CStatusMessage &message, int timeOutMs = -1);

        //! Info message, based on text message
        void showOverlayTextMessage(const BlackMisc::Network::CTextMessage &textMessage, int timeOutMs = -1);

        //! Image
        void showOverlayImage(const BlackMisc::CPixmap &image, int timeOutMs = -1);

        //! Image
        void showOverlayImage(const QPixmap &image, int timeOutMs = -1);

        //! Display one of the supported types
        void showOverlayVariant(const BlackMisc::CVariant &variant, int timeOutMs = -1);

        //! Close button clicked
        void close();

        //! Set the default confirmation button
        void setDefaultConfirmationButton(int button = QMessageBox::Cancel);

        //! Is awaiting a conformation
        bool hasPendingConfirmation() const;

    signals:
        //! Confirmation completed
        void confirmationCompleted();

    protected:
        //! Show message
        void display(int timeOutMs = -1);

        //! \copydoc QFrame::keyPressEvent
        virtual void keyPressEvent(QKeyEvent *event) override;

    private slots:
        //! Stylesheet changed
        void ps_onStyleSheetsChanged();

        //! OK clicked (only when confirmation bar is active)
        void ps_okClicked();

        //! Cancel clicked (only when confirmation bar is active)
        void ps_cancelClicked();

    private:
        QScopedPointer<Ui::COverlayMessages> ui;
        QString                              m_header;
        int                                  m_lastConfirmation = QMessageBox::Cancel;
        bool                                 m_awaitingConfirmation = false;
        std::function<void()>                m_okLambda;
        QTimer                               m_autoCloseTimer { this };
        QList<std::function<void()>> m_pendingMessageCalls;

        //! Init widget
        void init(int w, int h);

        //! Set header text
        void setHeader(const QString &header);

        //! Small
        bool useSmall() const;
    };
} // ns

#endif // guard
