#ifndef BLACKGUI_TEXTMESSAGECOMPONENT_H
#define BLACKGUI_TEXTMESSAGECOMPONENT_H

#include "blackgui/runtimebasedcomponent.h"
#include "blackgui/timerbasedcomponent.h"
#include "blackmisc/nwtextmessage.h"
#include "blackmisc/avaircraft.h"

#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>

namespace Ui { class CTextMessageComponent; }
namespace BlackGui
{
    //! Text message widget
    class CTextMessageComponent : public QTabWidget, public CRuntimeBasedComponent
    {
        Q_OBJECT

    public:
        //! Tabs
        enum Tab
        {
            TextMessagesAll,
            TextMessagesUnicom,
            TextMessagesCom1,
            TextMessagesCom2
        };

        //! Constructor
        explicit CTextMessageComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CTextMessageComponent();

        //! Set tooltip
        void setToolTip(const QString &tooltipText, Tab tab);

        //! SELCAL callback, SELCAL is obtained by that
        void setSelcalCallback(const std::function<const QString(void)> &selcalCallback) { this->m_selcalCallback = selcalCallback; }

    signals:
        //! Invisible text message
        void displayOverlayInfo(const BlackMisc::CStatusMessage &message) const;

    public slots:
        //! Command entered
        void commandEntered();

        /*!
         * \brief Append text messages (received, to be sent) to GUI
         * \param messages
         * \param sending
         */
        void appendTextMessagesToGui(const BlackMisc::Network::CTextMessageList &messages, bool sending = false);

    private:
        Ui::CTextMessageComponent *ui;
        QWidget *getTab(Tab tab); //!< enum to widget
        std::function<const QString(void)> m_selcalCallback; //!< obtain SELCAL by that
        QAction *m_clearTextEditAction;
        QTextEdit *m_currentTextEdit;

        /*!
         * \brief Add new text message tab
         * \param tabName   name of the new tab, usually the channel name
         * \return
         */
        QWidget *addNewTextMessageTab(const QString &tabName);

        //! Find text message tab by its name
        QWidget *findTextMessageTabByName(const QString &name) const;

        /*!
         * \brief Private channel text message
         * \param textMessage
         * \param sending   sending or receiving
         */
        void addPrivateChannelTextMessage(const BlackMisc::Network::CTextMessage &textMessage, bool sending = false);

        /*!
         * Stub for sending a text message (eihter radio or private message).
         * Sets sender / receiver depending on frequency / channel situation.
         */
        BlackMisc::Network::CTextMessage getTextMessageStubForChannel();

        //! own aircraft
        const BlackMisc::Aviation::CAircraft getOwnAircraft() const { Q_ASSERT(this->getIContextNetwork()); return this->getIContextNetwork()->getOwnAircraft(); }

        //! For this text message's recepient, is the current tab selected?
        bool isCorrespondingTextMessageTabSelected(BlackMisc::Network::CTextMessage textMessage) const;

        //! Network connected?
        bool isNetworkConnected() const { return this->getIContextNetwork() && this->getIContextNetwork()->isConnected() ; }

    private slots:
        //! Close text message tab
        void closeTextMessageTab();

        //! Context menu for text edit including clear
        void showContextMenuForTextEdit(const QPoint &pt);

        //! Clear text edit
        void clearTextEdit();

    };
}

#endif // guard
