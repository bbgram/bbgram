#pragma once

#include "Screen.h"
#include "../model/Chat.h"
#include "../model/ContactList.h"
#include "../model/User.h"
#include "../model/GroupChat.h"
#include "../utils/ContactManager.h"

#include <bb/cascades/TabbedPane>

class ApplicationUI;

class MainScreen : public Screen<bb::cascades::TabbedPane>
{
    Q_OBJECT
public:
    MainScreen(ApplicationUI* app);
    ~MainScreen();

    Q_INVOKABLE void sendMessage(Chat* chat, const QString& message);
    Q_INVOKABLE void deleteMessage(long long id);
    Q_INVOKABLE void markRead(Chat* chat);
    Q_INVOKABLE void createGroup(QVariantList users, const QString& title);
    Q_INVOKABLE void deleteChat(Chat* chat);
    Q_INVOKABLE void openFAQ();
    Q_INVOKABLE void sendInviteText(const QString& recipient, const QString& text);
    Q_INVOKABLE void dialANumber(const QString& number);
signals:
    void groupCreated(GroupChat* groupChat);
protected slots:
    void initialize();
protected:
    static MainScreen*  m_instance;
    ApplicationUI*      m_app;
    ContactManager*     m_contactManager;
    ContactList*        m_contacts;

    static void _createGroupCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
};
