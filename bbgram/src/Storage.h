#pragma once

#include <QtSql>
#include <bb/cascades/QListDataModel>

#include "model/Chat.h"
#include "model/GroupChat.h"
#include "model/Message.h"
#include "model/User.h"

class Storage: public QObject
{
public:
    Storage(QObject* parent);
    ~Storage();

    static Storage* instance();

    static void userUpdateHandler(struct tgl_state *TLS, struct tgl_user *U, unsigned flags);
    static void messageReceivedHandler(struct tgl_state *TLS, struct tgl_message *M);
    static void userStatusUpdateHandler(struct tgl_state *TLS, struct tgl_user *U);
    static void userTypingHandler(struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status);
    static void updateChatHandler(struct tgl_state *TLS, struct tgl_chat *C, unsigned flags);
    static void markedReadHandler(struct tgl_state *TLS, int num, struct tgl_message *list[]);

    bb::cascades::QListDataModel<User*>* contacts() const;
    bb::cascades::QListDataModel<Chat*>* dialogs() const;

    Message* getMessage(long long id);
    Chat* getPeer(int type, int id);

    void deleteMessage(long long id);
    void addContact(User* contact);
    void deleteContact(User* contact);

    void deleteHistory(Chat* chat);
    void deleteChat(Chat* chat);

    void updateChats();
    void updateUserInfo();
    void updateHistory(Chat* chat);
protected slots:
    void saveUpdatesToDatabase();
protected:
    static Storage* m_instance;
    QSqlDatabase    m_db;

    bb::cascades::QListDataModel<User*>*    m_contacts;
    bb::cascades::QListDataModel<Chat*>*    m_dialogs;

    QMap<long long, Chat*>                  m_peers;
    QMap<long long, Message*>               m_messages;

    QList<User*>    m_updatedUsers;
    QList<GroupChat*> m_updatedGroupChats;
    QTimer*         m_saveTimer;

    static void _getUserInfoCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *U);
    static void _getChatInfoCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C);
    static void _getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[]);
    static void _getHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[]);
    static void _deleteMessageCallback(struct tgl_state *TLS, void *callback_extra, int success);
    static void _deleteHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int offset);
    static void _updateGroupPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C);
    static void _updateContactPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *U);
};
