#include "Storage.h"

#include "model/User.h"
#include "model/GroupChat.h"

#include <QDateTime>

Storage* Storage::m_instance = 0;

const char* DATABASE_NAME = "data/storage.db";

using namespace bb::cascades;

Storage::Storage(QObject* parent)
    : QObject(parent)
{
    m_instance = this;

    QFile dbFile(DATABASE_NAME);
    bool createTables = !dbFile.exists();
    m_db = QSqlDatabase::addDatabase("QSQLITE", "storage");
    m_db.setDatabaseName(DATABASE_NAME);
    m_db.open();

    if (m_db.isOpen() && createTables)
    {
        QSqlQuery query(m_db);
        query.exec("CREATE TABLE users(id INTEGER PRIMARY KEY, name TEXT, last_seen INTEGER, data BLOB)");
        query.exec("CREATE TABLE contacts(user_id INTEGER PRIMARY KEY)");
        query.exec("CREATE TABLE dialogs(id INTEGER PRIMARY KEY, data BLOB)");
        query.exec("CREATE TABLE messages(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, to_type INTEGER, text TEXT, date INTEGER, data BLOB)");
    }

    m_contacts = new QListDataModel<User*>();
    m_contacts->setParent(this);
    m_dialogs = new QListDataModel<Chat*>();
    m_dialogs->setParent(this);

    QSqlQuery query(m_db);
    query.exec("SELECT id, data, last_seen FROM users");

    while (query.next())
    {
        int id = query.value(0).toInt();
        QByteArray data = query.value(1).toByteArray();


        User* user = (User*)getPeer(TGL_PEER_USER, id);
        user->deserialize(data);

        QDateTime lastSeen = QDateTime::fromTime_t( query.value(3).toInt());
        user->setStatus(0, lastSeen);
    }

    query.exec("SELECT user_id FROM contacts");
    while (query.next())
    {
        int id = query.value(0).toInt();
        User* user = (User*)getPeer(TGL_PEER_USER, id);
        m_contacts->append(user);
    }

    QList<Chat*> dialogs;
    query.exec("SELECT id, data FROM dialogs");
    while (query.next())
    {
        long long _id = query.value(0).toLongLong();
        int id = (int)_id;
        int type = (_id >> 32);

        if (type == TGL_PEER_USER)
        {
            Chat* chat = getPeer(type, id);
            dialogs.append(chat);
        }
        if (type == TGL_PEER_CHAT)
        {
            Chat* peer = new GroupChat(id);
            QByteArray data = query.value(1).toByteArray();
            ((GroupChat*)peer)->deserialize(data);
            long long peerId = ((long long)type << 32) | id;
            peer->setParent(this);
            m_peers.insert(peerId, peer);
            dialogs.append(peer);
        }
    }

    query.prepare("SELECT id, from_id, to_id, to_type, text, date, data FROM messages WHERE (to_id = :r_id AND to_type = :r_type) OR (to_id = :my_id AND to_type = 1 AND from_id = :s_id) ORDER BY date DESC LIMIT 50");
    for (int i = 0; i < dialogs.size(); i++)
    {
        Chat* chat = dialogs.value(i);
        query.bindValue(":r_type", chat->type());
        query.bindValue(":r_id", chat->id());
        query.bindValue(":my_id", gTLS->our_id);
        query.bindValue(":s_id", chat->id());
        query.exec();

        Message* lastMessage = 0;
        while (query.next())
        {
            long long id = query.value(0).toLongLong();
            Message* message = new Message(id);
            if (!lastMessage)
                lastMessage = message;
            message->m_fromId = query.value(1).toInt();
            message->m_toId = query.value(2).toInt();
            message->m_toType = query.value(3).toInt();
            message->m_text = query.value(4).toString();
            message->m_date = QDateTime::fromTime_t(query.value(5).toInt());
            QByteArray data = query.value(6).toByteArray();
            message->deserialize(data);
            m_messages.insert(id, message);
            chat->addMessage(message);
        }

        int newPos = m_dialogs->size();
        if (lastMessage)
        {
            for (int j = 0; j < m_dialogs->size(); j++)
            {
                Message* lm = m_dialogs->value(j)->lastMessage();
                if (!lm || (lm && lm->dateTime() < lastMessage->dateTime()))
                {
                    newPos = j;
                    break;
                }
            }
        }
        m_dialogs->insert(newPos, chat);
    }
}

Storage::~Storage()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
    m_instance = 0;
}

Storage* Storage::instance()
{
    return m_instance;
}

Message* Storage::getMessage(long long id)
{
    QMap<long long, Message*>::iterator it = m_messages.find(id);
    if (it != m_messages.end())
        return it.value();
    else
    {
        tgl_message* M = tgl_message_get(gTLS, id);
        if (M)
        {
            Message* message = new Message(id, M);
            message->setParent(this);
            m_messages.insert(id, message);

            QSqlDatabase &db = m_instance->m_db;
            QSqlQuery query(db);
            query.prepare("REPLACE INTO messages(id, from_id, to_id, to_type, text, date, data) VALUES(:id, :from_id, :to_id, :to_type, :text, :date, :data)");
            query.bindValue(":id", message->id());
            query.bindValue(":from_id", message->m_fromId);
            query.bindValue(":to_id", message->m_toId);
            query.bindValue(":to_type", message->m_toType);
            query.bindValue(":text", message->text());
            query.bindValue(":date", message->dateTime().toTime_t());
            QByteArray data = message->serialize();
            query.bindValue(":data", data);
            query.exec();

            return message;
        }
        else
            return 0;
    }
}

Chat* Storage::getPeer(int type, int id)
{
    long long peerId = ((long long)type << 32) | id;
    QMap<long long, Chat*>::iterator it = m_peers.find(peerId);

    if (it != m_peers.end())
        return it.value();
    else
    {
        Chat* peer = 0;
        if (type == TGL_PEER_USER)
            peer = new User(id);
        if (type == TGL_PEER_CHAT)
            peer = new GroupChat(id);
        if (peer)
        {
            m_peers.insert(peerId, peer);
            peer->setParent(this);
        }
        return peer;
    }
}

void Storage::deleteMessage(long long id)
{
    long long* msg_id = new long long();
    *msg_id = id;
    tgl_do_delete_msg(gTLS, id, Storage::_deleteMessageCallback, msg_id);
}

void Storage::addContact(User* contact)
{
    int idx = m_contacts->indexOf(contact);
    if (idx == -1)
        m_contacts->append(contact);
}

void Storage::deleteContact(User* contact)
{
    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);
    query.prepare("DELETE FROM contacts WHERE user_id=:id");
    query.bindValue(":id", contact->id());
    query.exec();

    QListDataModel<User*>* contacts = m_instance->m_contacts;
    int idx = contacts->indexOf(contact);
    if (idx != -1)
        contacts->removeAt(idx);
}

void load_photo_callback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    if (!success)
        return;

    Chat* chat = (Chat*)callback_extra;

    switch(chat->type())
    {
        case TGL_PEER_USER:
        {
            User* user = (User*)chat;
            user->setPhoto(filename);
        }
        break;
        case TGL_PEER_CHAT:
        {
            GroupChat* groupChat = (GroupChat*)chat;
            groupChat->setPhoto(filename);
        }
        break;
    }
}

void Storage::userUpdateHandler (struct tgl_state *TLS, struct tgl_user *U, unsigned flags)
{
    /*QString str;
#define CHECK_FLAG(FLAGS,F) if ((FLAGS & F) == F) str += " " #F;

    CHECK_FLAG(flags, TGL_UPDATE_CREATED)
    CHECK_FLAG(flags, TGL_UPDATE_DELETED)
    CHECK_FLAG(flags, TGL_UPDATE_PHONE)
    CHECK_FLAG(flags, TGL_UPDATE_CONTACT)
    CHECK_FLAG(flags, TGL_UPDATE_PHOTO)
    CHECK_FLAG(flags, TGL_UPDATE_BLOCKED)
    CHECK_FLAG(flags, TGL_UPDATE_REAL_NAME)
    CHECK_FLAG(flags, TGL_UPDATE_NAME)
    str += "  %  ";

    CHECK_FLAG(U->flags, FLAG_MESSAGE_EMPTY)
    CHECK_FLAG(U->flags, FLAG_DELETED)
    CHECK_FLAG(U->flags, FLAG_FORBIDDEN)
    CHECK_FLAG(U->flags, FLAG_HAS_PHOTO)
    CHECK_FLAG(U->flags, FLAG_CREATED)
    CHECK_FLAG(U->flags, FLAG_SESSION_OUTBOUND)
    CHECK_FLAG(U->flags, FLAG_USER_SELF)
    CHECK_FLAG(U->flags, FLAG_USER_FOREIGN)
    CHECK_FLAG(U->flags, FLAG_USER_CONTACT)
    CHECK_FLAG(U->flags, FLAG_USER_IN_CONTACT)
    CHECK_FLAG(U->flags, FLAG_USER_OUT_CONTACT)
    CHECK_FLAG(U->flags, FLAG_ENCRYPTED)
    CHECK_FLAG(U->flags, FLAG_PENDING)

    qDebug() << "update_user_handler user=" << QString::fromUtf8(U->first_name) << " flags=" << flags << str;*/

    User* user = (User*)m_instance->getPeer(TGL_PEER_USER, U->id.id);

    if (flags & TGL_UPDATE_PHONE)
        user->setPhone(QString::fromUtf8(U->phone));

    if (flags & TGL_UPDATE_NAME)
        user->setName(QString::fromUtf8(U->first_name), QString::fromUtf8(U->last_name));

    if (flags & TGL_UPDATE_PHOTO)
    {
        if (U->photo.sizes_num != 0)
            tgl_do_load_photo(gTLS, &U->photo, load_photo_callback, user);
        /*else
            user->setPhoto("");*/
    }

    if (flags & TGL_UPDATE_CONTACT)
    {
        if ((U->flags & FLAG_USER_CONTACT) == 0)
        {
            m_instance->deleteContact(user);
        }

        if (U->flags & FLAG_USER_CONTACT)
        {
            m_instance->addContact(user);
        }

        tgl_peer_id_t peer;
        peer.type = user->type();
        peer.id = user->id();
        tgl_do_get_user_info(gTLS, peer, false, NULL, NULL);
    }

    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);
    query.prepare("REPLACE INTO users(id, name, data) VALUES(:id, :name, :data)");
    query.bindValue(":id", user->id());
    query.bindValue(":name", user->name());
    QByteArray data = user->serialize();
    query.bindValue(":data", data);
    query.exec();
}

void Storage::userStatusUpdateHandler(struct tgl_state *TLS, struct tgl_user *U)
{
    bool online = U->status.online == 1;
    QDateTime lastSeen = QDateTime::fromTime_t(U->status.when);
    User* user = (User*)m_instance->getPeer(TGL_PEER_USER, U->id.id);
    user->setStatus(online, lastSeen);
}

void Storage::userTypingHandler(struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status)
{
    User* user = (User*)m_instance->getPeer(TGL_PEER_USER, U->id.id);
    if (user)
        user->setTypingStatus(status);
}

void Storage::messageReceivedHandler(struct tgl_state *TLS, struct tgl_message *M)
{
    int peer_type = M->to_id.type;
    int peer_id = M->to_id.id;

    if (M->to_id.type == TGL_PEER_USER && M->to_id.id == gTLS->our_id)
    {
        peer_id = M->from_id.id;    // in
        User* user = (User*)m_instance->getPeer(TGL_PEER_USER, peer_id);
        if (user)
            user->resetTypingStatus();
    }
    if (M->from_id.type == TGL_PEER_USER && M->from_id.id == gTLS->our_id)
        peer_id = M->to_id.id;      // out

    Chat* chat = m_instance->getPeer(peer_type, peer_id);
    Message* message = m_instance->getMessage(M->id);

    GroupDataModel* messages = chat->m_messages;
    if (!messages->find(message).isEmpty())
        return;

    messages->insert(message);

    Message* lastMessage = chat->lastMessage();

    QListDataModel<Chat*>* dialogs = m_instance->m_dialogs;
    int chatIdx = -1;
    for (int i = 0; i < dialogs->size(); i++)
    {
        if (dialogs->value(i) == chat)
        {
            chatIdx = i;
            break;
        }
    }

    if (chatIdx == -1)
        dialogs->insert(dialogs->size(), chat);
    else if (lastMessage == message)
    {
        int newPos = dialogs->size();
        for (int j = 0; j < dialogs->size(); j++)
        {
            Message* lm = dialogs->value(j)->lastMessage();
            if (lm && lm->dateTime() < message->dateTime())
            {
                newPos = j;
                break;
            }
        }
        dialogs->move(chatIdx, newPos);
    }
}

void Storage::updateChatHandler(struct tgl_state *TLS, struct tgl_chat *C, unsigned flags)
{
    /*QString str;
    #define CHECK_FLAG(FLAGS,F) if ((FLAGS & F) == F) str += " " #F;

        CHECK_FLAG(flags, TGL_UPDATE_CREATED)
        CHECK_FLAG(flags, TGL_UPDATE_DELETED)
        CHECK_FLAG(flags, TGL_UPDATE_PHONE)
        CHECK_FLAG(flags, TGL_UPDATE_CONTACT)
        CHECK_FLAG(flags, TGL_UPDATE_PHOTO)
        CHECK_FLAG(flags, TGL_UPDATE_BLOCKED)
        CHECK_FLAG(flags, TGL_UPDATE_REAL_NAME)
        CHECK_FLAG(flags, TGL_UPDATE_NAME)
        str += "  %  ";

        CHECK_FLAG(C->flags, FLAG_MESSAGE_EMPTY)
        CHECK_FLAG(C->flags, FLAG_DELETED)
        CHECK_FLAG(C->flags, FLAG_FORBIDDEN)
        CHECK_FLAG(C->flags, FLAG_HAS_PHOTO)
        CHECK_FLAG(C->flags, FLAG_CREATED)
        CHECK_FLAG(C->flags, FLAG_SESSION_OUTBOUND)
        CHECK_FLAG(C->flags, FLAG_USER_SELF)
        CHECK_FLAG(C->flags, FLAG_USER_FOREIGN)
        CHECK_FLAG(C->flags, FLAG_USER_CONTACT)
        CHECK_FLAG(C->flags, FLAG_USER_IN_CONTACT)
        CHECK_FLAG(C->flags, FLAG_USER_OUT_CONTACT)
        CHECK_FLAG(C->flags, FLAG_ENCRYPTED)
        CHECK_FLAG(C->flags, FLAG_PENDING)

        qDebug() << "Storage::updateChatHandler user=" << QString::fromUtf8(C->title) << " flags=" << flags << str;*/

    GroupChat* groupChat = (GroupChat*)m_instance->getPeer(TGL_PEER_CHAT, C->id.id);

    if (flags & TGL_UPDATE_TITLE)
        groupChat->setTitle(QString::fromUtf8(C->title));

    if (flags & TGL_UPDATE_PHOTO)
    {
        if (C->photo.sizes_num != 0)
            tgl_do_load_photo(gTLS, &C->photo, load_photo_callback, groupChat);
        /*else
            groupChat->setPhoto("");*/
    }

    if (flags & TGL_UPDATE_ADMIN)
    {
        groupChat->setAdmin(C->admin_id);
    }

    if (flags & TGL_UPDATE_MEMBERS)
    {
        groupChat->setMembers(C->user_list, C->user_list_size);
    }

    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);

    long long id = ((long long)C->id.type << 32) | C->id.id;
    query.prepare("REPLACE INTO dialogs(id, data) VALUES(:id, :data)");
    query.bindValue(":id", id);
    QByteArray data = groupChat->serialize();
    query.bindValue(":data", data);
    query.exec();
}

void Storage::markedReadHandler(struct tgl_state *TLS, int num, struct tgl_message *list[])
{
    for (int i = 0; i < num; i++)
    {
        Message* message = m_instance->getMessage(list[i]->id);
        message->markAsRead();
    }
    //! update db
}

QListDataModel<User*>* Storage::contacts() const
{
    return m_contacts;
}

QListDataModel<Chat*>* Storage::dialogs() const
{
    return m_dialogs;
}

void Storage::_getContactsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *contacts[])
{
    if (!success)
        return;

    QSqlDatabase &db = m_instance->m_db;
    db.transaction();
    QSqlQuery query(db);
    query.exec("DELETE FROM contacts");
    query.prepare("REPLACE INTO contacts(user_id) VALUES(:id)");
    for (int i = 0; i < size; i++)
    {
        query.bindValue(":id", contacts[i]->id.id);
        query.exec();
    }
    db.commit();

    QListDataModel<User*>* newContacts = m_instance->m_contacts;
    QList<User*> oldContacts;
    for (int i = 0; i < newContacts->size(); i++)
        oldContacts.append(newContacts->value(i));

    for (int i = 0; i < size; i++)
    {
        User* contact = (User*)m_instance->getPeer(TGL_PEER_USER, contacts[i]->id.id);
        //tgl_do_get_user_info(gTLS, contacts[i]->id, false, NULL, NULL);
        if (newContacts->indexOf(contact) != -1)
            oldContacts.removeAll(contact);
        else
            newContacts->append(contact);
    }

    for (int i = 0; i < oldContacts.size(); i++)
    {
        int idx =  newContacts->indexOf(oldContacts.at(i));
        newContacts->removeAt(idx);
    }
}

void Storage::_getUserInfoCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *U)
{
    if (!success)
        return;

    User* user = (User*)m_instance->getPeer(U->id.type, U->id.id);
    Message* lastMessage = 0;
    if (U->last)
    {
        lastMessage = m_instance->getMessage(U->last->id);
        if (lastMessage != user->lastMessage())
            user->addMessage(lastMessage);
    }
    int idx = 0;
    for (int i = 0; i < m_instance->m_dialogs->size(); i++)
    {
        Chat* chat = m_instance->m_dialogs->value(i);
        if (chat == user)
        {
            idx = -1;
            break;
        }
        Message* chatLastMessage = chat->lastMessage();
        if (!lastMessage || (chatLastMessage && chatLastMessage->dateTime() > lastMessage->dateTime()))
            idx++;
    }
    if (idx != -1)
        m_instance->m_dialogs->insert(idx, user);
    m_instance->updateHistory(user);
}

void Storage::_getChatInfoCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C)
{
    if (!success)
        return;

    GroupChat* groupChat = (GroupChat*)m_instance->getPeer(C->id.type, C->id.id);

    Message* lastMessage = 0;
    if (C->last)
    {
        lastMessage = m_instance->getMessage(C->last->id);
        if (lastMessage != groupChat->lastMessage())
            groupChat->addMessage(lastMessage);
    }
    int idx = 0;
    for (int i = 0; i < m_instance->m_dialogs->size(); i++)
    {
        Chat* chat = m_instance->m_dialogs->value(i);
        if (chat == groupChat)
        {
            idx = -1;
            break;
        }
        Message* chatLastMessage = chat->lastMessage();
        if (!lastMessage || (chatLastMessage && chatLastMessage->dateTime() > lastMessage->dateTime()))
            idx++;
    }
    if (idx != -1)
        m_instance->m_dialogs->insert(idx, groupChat);
    m_instance->updateHistory(groupChat);
}

void Storage::_getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[])
{
    if (!success)
        return;

    Storage* _this = (Storage*)callback_extra;

    //_this->m_dialogs->clear();
    for (int i = 0; i < size; i++)
    {
        tgl_peer_id_t _peer = peers[i];
        int peer_type = _peer.type;
        int _last_msg_id = last_msg_id[i];
        int _unread_count = unread_count[i];
        //tgl_do_get_history_ext(gTLS, _peer, _last_msg_id, 1, 0, get_history_callback, 0);

        if (peer_type == TGL_PEER_USER)
            tgl_do_get_user_info(gTLS, _peer, 0, _getUserInfoCallback, _this);
        else if (peer_type == TGL_PEER_CHAT)
            tgl_do_get_chat_info(gTLS, _peer, 0, _getChatInfoCallback, _this);


        long long id = ((long long)_peer.type << 32) | _peer.id;
        QSqlDatabase &db = m_instance->m_db;
        QSqlQuery query(db);
        query.prepare("REPLACE INTO dialogs(id) VALUES(:id)");
        query.bindValue(":id", id);
        query.exec();
    }
}

void Storage::_getHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[])
{
    if (!success)
        return;

    Chat* chat = (Chat*)callback_extra;
    GroupDataModel* messages = chat->m_messages;
    //messages->clear();
    for (int i = 0; i < size; i++)
    {
        Message* message = m_instance->getMessage(list[i]->id);
        //messages->insert(message);
    }
}

void Storage::_deleteMessageCallback(struct tgl_state *TLS, void *callback_extra, int success)
{
    long long id = *(long long*)callback_extra;
    delete (long long*)callback_extra;

    Message* msg = m_instance->getMessage(id);

    for (int i = 0; i < m_instance->m_dialogs->size(); i++)
        m_instance->m_dialogs->value(i)->deleteMessage(msg);

    m_instance->m_messages.remove(id);
}

void Storage::updateChats()
{
    tgl_do_get_dialog_list(gTLS, _getDialogsCallback, this);
}

void Storage::updateUserInfo()
{
    tgl_peer_id_t userId = {TGL_PEER_USER, gTLS->our_id};
    tgl_do_get_user_info(gTLS, userId, false, NULL, NULL);
}

void Storage::updateHistory(Chat* chat)
{
    tgl_peer_id_t peer;
    peer.type = chat->type();
    peer.id = chat->id();
    tgl_do_get_history(gTLS, peer, 50, 0, _getHistoryCallback, chat);
}
