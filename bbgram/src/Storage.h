#pragma once

#include <QtSql>
#include <bb/cascades/QListDataModel>

#include "model/Peer.h"
#include "model/GroupChat.h"
#include "model/Message.h"
#include "model/User.h"

class Storage: public QObject
{
    Q_OBJECT
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
    static void messagesDeletedHandler(struct tgl_state *TLS, int num, int list[]);

    bb::cascades::QListDataModel<User*>* contacts() const;
    bb::cascades::QListDataModel<Peer*>* dialogs() const;

    Message* getMessage(long long id);
    Peer* getPeer(int type, int id);

    void deleteMessage(long long id);
    void addContact(User* contact);
    void deleteContact(User* contact);

    void deleteHistory(Peer* peer);
    void deleteChat(Peer* peer);

    void updateChats();
    void updateUserInfo();

    void loadAdditionalHistory(Peer* peer);
    void searchMessage(Peer* peer, int from, int to, int limit, int offset, const char *s);
signals:
    void newMessageReceived(const Message* message);
protected slots:
    void saveUpdatesToDatabase();
protected:
    static Storage* m_instance;
    QSqlDatabase    m_db;

    bb::cascades::QListDataModel<User*>*    m_contacts;
    bb::cascades::QListDataModel<Peer*>*    m_dialogs;

    QMap<long long, Peer*>                  m_peers;
    QMap<long long, Message*>               m_messages;

    QList<Peer*>    m_updatedPeers;
    QTimer*         m_saveTimer;

    static void _getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[]);
    static void _getHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[]);
    static void _deleteMessageCallback(struct tgl_state *TLS, void *callback_extra, int success);
    static void _deleteHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int offset);
    static void _updateGroupPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C);
    static void _updateContactPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *U);
    static void _searchMessageCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[]);
    static void _loadPhotoCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename);
};
