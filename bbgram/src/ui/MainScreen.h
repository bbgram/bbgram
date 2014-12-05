#pragma once

#include "Screen.h"
#include "../model/Chat.h"
#include "../model/ContactList.h"
#include "../model/User.h"
#include "../model/GroupChat.h"

#include <bb/cascades/TabbedPane>

class ApplicationUI;

class MainScreen : public Screen<bb::cascades::TabbedPane>
{
    Q_OBJECT
public:
    MainScreen(ApplicationUI* app);
    ~MainScreen();

    Q_INVOKABLE void addContact(const QString& firstName, const QString& lastName, const QString& phone);
    Q_INVOKABLE void renameContact(const QString& firstName, const QString& lastName, const QString& phone);
    Q_INVOKABLE void deleteContact(User* contact);
    Q_INVOKABLE void sendMessage(Chat* chat, const QString& message);
    Q_INVOKABLE void sendPhoto(Chat* chat, const QString& fileName);
    Q_INVOKABLE void deleteMessage(long long id);
    Q_INVOKABLE void markRead(Chat* chat);
    Q_INVOKABLE void createGroup(QVariantList users, const QString& title, const QString& chatPhoto);
    Q_INVOKABLE void setGroupName(GroupChat* group, const QString& title);
    Q_INVOKABLE void addUserToGroup(GroupChat* group, User* user);
    Q_INVOKABLE void deleteUserFromGroup(GroupChat* group, User* user);
    Q_INVOKABLE void deleteHistory(Chat* chat);
    Q_INVOKABLE void deleteChat(Chat* chat);
    Q_INVOKABLE void openFAQ();
    Q_INVOKABLE void sendInviteText(const QString& recipient, const QString& text);
    Q_INVOKABLE void dialANumber(const QString& number);
    Q_INVOKABLE User* getUser(int id);
signals:
    void contactAdded(bool error, QString message);
    void contactRenamed(bool error, QString message);
    void contactDeleted(bool error, QString message);
    void groupCreated(GroupChat* groupChat);
protected slots:
    void initialize();
protected:
    static MainScreen*  m_instance;
    ApplicationUI*      m_app;
    ContactList*        m_contacts;

    bool contactExist(const QString& phone);

    static void _createGroupCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
    static void _addMemberCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
    static void _deleteMemberCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
    static void _contactAddHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[]);
    static void _contactRenameHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[]);
    static void _contactDeleteHandler(struct tgl_state *TLS, void *callback_extra, int success);
};
