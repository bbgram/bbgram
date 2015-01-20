#pragma once

#include "Screen.h"
#include "../model/Peer.h"
#include "../model/ContactList.h"
#include "../model/ChatList.h"
#include "../model/User.h"
#include "../model/GroupChat.h"
#include "../model/BroadcastChat.h"

#include <bb/cascades/TabbedPane>
#include <bb/platform/Notification>

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
    Q_INVOKABLE void setProfilePhoto(const QString& fileName);
    Q_INVOKABLE void sendMessage(Peer* peer, const QString& message);
    Q_INVOKABLE void sendPhoto(Peer* peer, const QString& fileName);
    Q_INVOKABLE void deleteMessage(long long id);
    Q_INVOKABLE void markRead(Peer* peer);
    Q_INVOKABLE void createGroup(QVariantList users, const QString& title, const QString& chatPhoto);
    Q_INVOKABLE QVariant createBroadcast(QVariantList users);
    Q_INVOKABLE void setGroupName(GroupChat* group, const QString& title);
    Q_INVOKABLE void addUserToGroup(GroupChat* group, User* user);
    Q_INVOKABLE void deleteMemberFromGroup(GroupChat* group, User* member);
    Q_INVOKABLE void deleteHistory(Peer* peer);
    Q_INVOKABLE void deleteChat(Peer* peer);
    Q_INVOKABLE void openFAQ();
    Q_INVOKABLE void sendInviteText(const QString& recipient, const QString& text);
    Q_INVOKABLE void dialANumber(const QString& number);
    Q_INVOKABLE User* getUser(int id);
    Q_INVOKABLE QString getAppVersion() const;

    static MainScreen* instance();
signals:
    void contactAdded(bool error, QString message);
    void contactRenamed(bool error, QString message);
    void contactDeleted(bool error, QString message);
    void groupCreated(GroupChat* groupChat);
protected slots:
    void initialize();
    void onAppFullScreen();
    void onAppThumbnail();
    void showNotifications();
    void onMessageReceived(const Message* message);
protected:
    static MainScreen*  m_instance;
    ApplicationUI*      m_app;
    ContactList*        m_contacts;
    ChatList*           m_dialogs;

    QTimer m_notificationTimer;
    QList<bb::platform::Notification*> m_notificationList;

    bool                m_appFullScreen;

    bool contactExist(const QString& phone);

    static void _createGroupCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
    static void _addMemberCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
    static void _deleteMemberCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
    static void _deleteSelfFromGroupCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M);
    static void _contactAddHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[]);
    static void _contactRenameHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[]);
    static void _contactDeleteHandler(struct tgl_state *TLS, void *callback_extra, int success);
    static void _markReaded(struct tgl_state *TLS, void *callback_extra, int success);
};
