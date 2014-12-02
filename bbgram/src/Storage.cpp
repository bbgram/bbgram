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
        query.exec("CREATE TABLE users(id INTEGER PRIMARY KEY, name TEXT, data BLOB)");
        query.exec("CREATE TABLE contacts(user_id INTEGER PRIMARY KEY)");
        query.exec("CREATE TABLE dialogs(id INTEGER PRIMARY KEY, type INTEGER, unread_count INTEGER, last_msg INTEGER)");
        query.exec("CREATE TABLE statuses(user_id INTEGER PRIMARY KEY, online INTEGER, last_seen INTEGER)");
    }

    m_contacts = new QListDataModel<User*>();
    m_contacts->setParent(this);
    m_dialogs = new QListDataModel<Chat*>();
    m_dialogs->setParent(this);

    QSqlQuery query(m_db);
    query.exec("SELECT id, data, online, last_seen FROM users LEFT OUTER JOIN statuses ON users.id = statuses.user_id");

    while (query.next())
    {
        int id = query.value(0).toInt();
        QByteArray data = query.value(1).toByteArray();

        User* user = addUser(id);
        user->deserialize(data);

        bool online =  query.value(2).toInt() == 1;
        QDateTime lastSeen = QDateTime::fromTime_t( query.value(3).toInt());
        user->setStatus(online, lastSeen);
    }

    query.exec("SELECT user_id FROM contacts");
    while (query.next())
    {
        int id = query.value(0).toInt();
        User* user = addUser(id);
        m_contacts->append(user);
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

User* Storage::addUser(int id)
{
    long long peerId = (TGL_PEER_USER << 32) | id;
    QMap<long long, Chat*>::iterator it = m_peers.find(peerId);
    if (it != m_peers.end())
        return (User*)it.value();
    else
    {
        User* user = new User(id);
        user->setParent(this);
        m_peers.insert(peerId, user);
        return user;
    }
}

User* Storage::findUser(int id)
{
    long long peerId = (TGL_PEER_USER << 32) | id;
    QMap<long long, Chat*>::iterator it = m_peers.find(peerId);
    if (it != m_peers.end())
        return (User*)it.value();
    else
        return 0;
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
            return message;
        }
        else
            return 0;
    }
}

Chat* Storage::getPeer(int type, int id)
{
    long long peerId = (type << 32) | id;
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
            m_peers.insert(peerId, peer);
        return peer;
    }
}

void Storage::deleteMessage(long long id)
{
    long long* msg_id = new long long();
    *msg_id = id;
    tgl_do_delete_msg(gTLS, id, Storage::_deleteMessageCallback, msg_id);
}

void load_photo_callback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    if (!success)
        return;

    User* _this = (User*)callback_extra;
    _this->setPhoto(filename);
}

void Storage::userUpdateHandler (struct tgl_state *TLS, struct tgl_user *U, unsigned flags)
{
/*    QString str;
#define CHECK_FLAG(F) if ((flags & F) == F) str += " " #F;

            CHECK_FLAG(TGL_UPDATE_CREATED)
    CHECK_FLAG(TGL_UPDATE_DELETED)
            CHECK_FLAG(TGL_UPDATE_PHONE)
    CHECK_FLAG(TGL_UPDATE_CONTACT)
            CHECK_FLAG(TGL_UPDATE_PHOTO)
    CHECK_FLAG(TGL_UPDATE_BLOCKED)
    CHECK_FLAG(TGL_UPDATE_REAL_NAME)
            CHECK_FLAG(TGL_UPDATE_NAME)

    qDebug() << "update_user_handler user=" << QString::fromUtf8(U->first_name) << " flags=" << flags << str;

*/
    User* user = 0;
    if (flags & TGL_UPDATE_CREATED)
    {
        user = m_instance->addUser(U->id.id);
    }
    else
        user = m_instance->findUser(U->id.id);

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

    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);
    query.prepare("REPLACE INTO users(id, name, data) VALUES(:id, :name, :data)");
    query.bindValue(":id", user->id());
    query.bindValue(":name", user->name());
    query.bindValue(":data", user->serialize());
    query.exec();
}

void Storage::userStatusUpdateHandler(struct tgl_state *TLS, struct tgl_user *U)
{
    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);
    query.prepare("REPLACE INTO statuses(user_id, online, last_seen) VALUES(:id, :online, :last_seen)");
    query.bindValue(":id", U->id.id);
    query.bindValue(":online", U->status.online);
    query.bindValue(":last_seen", U->status.when);
    query.exec();

    bool online = U->status.online == 1;
    QDateTime lastSeen = QDateTime::fromTime_t(U->status.when);
    User* user = m_instance->findUser(U->id.id);
    user->setStatus(online, lastSeen);
}

void Storage::userTypingHandler(struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status)
{
    User* user = m_instance->findUser(U->id.id);
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
        User* user = m_instance->findUser(peer_id);
        if (user)
            user->resetTypingStatus();
    }
    if (M->from_id.type == TGL_PEER_USER && M->from_id.id == gTLS->our_id)
        peer_id = M->to_id.id;      // out

    Chat* chat = m_instance->getPeer(peer_type, peer_id);
    Message* message = m_instance->getMessage(M->id);

    QListDataModel<Message*>* messages = chat->m_messages;
    int msgIdx = -1;
    for (int i = 0; i < messages->size(); i++)
    {
        Message* existingMessage = messages->value(i);
        if (existingMessage->id() == message->id())
            return;
        if (existingMessage->date() < message->date())
        {
            msgIdx = i;
            messages->insert(i, message);
            break;
        }
    }
    if (msgIdx == -1)
    {
        messages->append(message);
        msgIdx = messages->size() - 1;
    }

    if (msgIdx == 0)
    {
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

        int newPos = dialogs->size() - 1;
        for (int j = 0; j < dialogs->size(); j++)
        {
            Message* lastMessage = dialogs->value(j)->lastMessage();
            if (lastMessage && lastMessage->date() < message->date())
            {
                newPos = j;
                break;
            }
        }
        if (chatIdx != -1)
            dialogs->move(chatIdx, newPos);
        else
            dialogs->insert(newPos, chat);
    }
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
        User* contact = m_instance->findUser(contacts[i]->id.id);
        tgl_do_get_user_info(gTLS, contacts[i]->id, false, NULL, NULL);
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
        if (lastMessage)
            user->addMessage(lastMessage);
    }
    int idx = 0;
    for (int i = 0; i < m_instance->m_dialogs->size(); i++)
    {
        Chat* chat = m_instance->m_dialogs->value(i);
        Message* chatLastMessage = chat->lastMessage();
        if (!lastMessage || (chatLastMessage && chatLastMessage->date() > lastMessage->date()))
            idx++;
    }
    m_instance->m_dialogs->insert(idx, user);
    m_instance->updateHistory(user);
}

void Storage::_getChatInfoCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C)
{
    if (!success)
        return;

    GroupChat* groupChat = (GroupChat*)m_instance->getPeer(C->id.type, C->id.id);
    if (!groupChat)
        groupChat = new GroupChat(C->id.id);
    groupChat->update(C);

    Message* lastMessage = 0;
    if (C->last)
    {
        lastMessage = m_instance->getMessage(C->last->id);
        if (lastMessage)
            groupChat->addMessage(lastMessage);
    }
    int idx = 0;
    for (int i = 0; i < m_instance->m_dialogs->size(); i++)
    {
        Chat* chat = m_instance->m_dialogs->value(i);
        Message* chatLastMessage = chat->lastMessage();
        if (!lastMessage || (chatLastMessage && chatLastMessage->date() > lastMessage->date()))
            idx++;
    }
    m_instance->m_dialogs->insert(idx, groupChat);
    m_instance->updateHistory(groupChat);
}

void Storage::_getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[])
{
    if (!success)
        return;

    Storage* _this = (Storage*)callback_extra;

    _this->m_dialogs->clear();
    for (int i = 0; i < size; i++)
    {
        tgl_peer_id_t _peer = peers[i];
        int peer_type = peers[i].type;
        int _last_msg_id = last_msg_id[i];
        int _unread_count = unread_count[i];
        //tgl_do_get_history_ext(gTLS, _peer, _last_msg_id, 1, 0, get_history_callback, 0);

        if (peer_type == TGL_PEER_USER)
            tgl_do_get_user_info(gTLS, _peer, 0, _getUserInfoCallback, _this);
        else if (peer_type == TGL_PEER_CHAT)
            tgl_do_get_chat_info(gTLS, _peer, 0, _getChatInfoCallback, _this);
    }
}

void Storage::_getHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[])
{
    if (!success)
        return;

    Chat* chat = (Chat*)callback_extra;
    QListDataModel<Message*>* messages = chat->m_messages;
    messages->clear();
    for (int i = 0; i < size; i++)
    {
        Message* message = m_instance->getMessage(list[i]->id);
        messages->append(message);
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

void Storage::updateContacts()
{
    tgl_do_update_contact_list(gTLS, _getContactsCallback, this);
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

