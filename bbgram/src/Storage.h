#pragma once

#include <QtSql>
#include <bb/cascades/QListDataModel>

#include "model/Chat.h"
#include "model/Message.h"
#include "model/User.h"

class Storage: public QObject
{
public:
    Storage(QObject* parent);
    ~Storage();

    static Storage* instance();

    static void userUpdateHandler(struct tgl_state *TLS, struct tgl_user *U, unsigned flags);
    static void userStatusUpdateHandler(struct tgl_state *TLS, struct tgl_user *U);

    bb::cascades::QListDataModel<User*>* contacts() const;
    bb::cascades::QListDataModel<Chat*>* chats() const;

    User* addUser(int id);
    User* findUser(int id);

    Message* getMessage(long long id);

    void updateContacts();
    void updateChats();
protected:
    static Storage* m_instance;
    QSqlDatabase    m_db;

    bb::cascades::QListDataModel<User*>*    m_contacts;
    bb::cascades::QListDataModel<Chat*>*    m_chats;
    QMap<int, User*>                        m_users;
    QMap<long long, Message*>               m_messages;

    static void _getContactsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *contacts[]);
    static void _getUserInfoCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *U);
    static void _getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[]);
};
