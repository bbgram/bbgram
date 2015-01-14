#include "Storage.h"

#include "model/User.h"
#include "model/GroupChat.h"
#include "model/BroadcastChat.h"

#include <QDateTime>

Storage* Storage::m_instance = 0;

const int MAX_PHOTOS_TO_LOAD = 5;
const int HISTORY_LIMIT = 20;
const char* DATABASE_NAME = "data/storage.db";

using namespace bb::cascades;

Storage::Storage(QObject* parent)
    : QObject(parent), m_photosInQuoue(0)
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
        query.exec("CREATE TABLE peers(id INTEGER PRIMARY KEY, data BLOB)");
        query.exec("CREATE TABLE contacts(user_id INTEGER PRIMARY KEY)");
        query.exec("CREATE TABLE dialogs(id INTEGER PRIMARY KEY)");
        query.exec("CREATE TABLE messages(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, to_type INTEGER, text TEXT, date INTEGER, data BLOB)");

        query.exec("CREATE INDEX messages_to_id_idx ON messages(to_id);");
        query.exec("CREATE INDEX messages_to_id_to_type_idx ON messages(to_id, to_type);");
        query.exec("CREATE INDEX messages_to_id_to_type_from_id_idx ON messages(to_id, to_type, from_id);");
    }

    m_contacts = new PeerDataModel();
    m_contacts->setParent(this);
    m_dialogs = new PeerDataModel();
    m_dialogs->setParent(this);

    QSqlQuery query(m_db);
    query.exec("SELECT id, data FROM peers");

    while (query.next())
    {
        long long peerId = query.value(0).toLongLong();
        int id = (int)peerId;
        int type = (peerId >> 32);

        Peer* peer = (Peer*)getPeer(type, id);
        QByteArray data = query.value(1).toByteArray();
        peer->deserialize(data);
    }

    query.exec("SELECT user_id FROM contacts");
    while (query.next())
    {
        int id = query.value(0).toInt();
        User* user = (User*)getPeer(TGL_PEER_USER, id);
        m_contacts->append(user);
    }

    QList<Peer*> dialogs;
    query.exec("SELECT id FROM dialogs");
    while (query.next())
    {
        long long peerId = query.value(0).toLongLong();
        int id = (int)peerId;
        int type = (peerId >> 32);

        Peer* peer = getPeer(type, id);
        dialogs.append(peer);
    }

    query.prepare("SELECT id, from_id, to_id, to_type, text, date, data FROM messages WHERE (to_id = :r_id AND to_type = :r_type) OR (to_id = :my_id AND to_type = 1 AND from_id = :s_id) ORDER BY date DESC LIMIT " + QString::number(HISTORY_LIMIT));
    for (int i = 0; i < dialogs.size(); i++)
    {
        Peer* peer = dialogs.value(i);
        query.bindValue(":r_type", peer->type());
        query.bindValue(":r_id", peer->id());
        query.bindValue(":my_id", gTLS->our_id);
        query.bindValue(":s_id", peer->id());
        query.exec();

        Message* lastMessage = 0;
        while (query.next())
        {
            long long id = query.value(0).toLongLong();
            Message* message = new Message(id);
            message->setParent(this);
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
            peer->addMessage(message);
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
        m_dialogs->insert(newPos, peer);
    }

    m_saveTimer = new QTimer(this);
    Storage::connect(m_saveTimer, SIGNAL(timeout()), this, SLOT(saveUpdatesToDatabase()));
    m_saveTimer->start(1000);
}

Storage::~Storage()
{
    saveUpdatesToDatabase();
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

Peer* Storage::getPeer(int type, int id)
{
    long long peerId = ((long long)type << 32) | id;
    QMap<long long, Peer*>::iterator it = m_peers.find(peerId);

    if (it != m_peers.end())
        return it.value();
    else
    {
        Peer* peer = 0;
        if (type == TGL_PEER_USER)
            peer = new User(id);
        if (type == TGL_PEER_CHAT)
            peer = new GroupChat(id);
        if (type == TGL_BROADCAST_CHAT)
            peer = new BroadcastChat();
        if (type == TGL_PEER_ENCR_CHAT)
            peer = new EncrChat(id);
        if (peer)
        {
            m_peers.insert(peerId, peer);
            peer->setParent(this);
        }
        return peer;
    }
}

void Storage::markPeerDirty(Peer* peer)
{
    if (m_updatedPeers.indexOf(peer) == -1)
        m_updatedPeers.append(peer);
}

void Storage::deleteMessage(long long id)
{
    tgl_do_delete_msg(gTLS, id, Storage::_deleteMessageCallback, (void*)(int)id);
}

void Storage::addContact(User* contact)
{
    if (m_contacts->indexOf(contact) == -1 && contact->id() != gTLS->our_id)
    {
        m_contacts->append(contact);
        markPeerDirty(contact);
    }
}

void Storage::deleteContact(User* contact)
{
    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);
    query.prepare("DELETE FROM contacts WHERE user_id=:id");
    query.bindValue(":id", contact->id());
    query.exec();

    int idx = m_instance->m_contacts->indexOf(contact);
    if (idx != -1)
        m_instance->m_contacts->removeAt(idx);
}

void Storage::deleteHistory(Peer* peer)
{
    tgl_do_delete_history(gTLS, {peer->type(), peer->id()}, 0, Storage::_deleteHistoryCallback, peer);
}

void Storage::deleteChat(Peer* peer)
{
    int idx = m_dialogs->indexOf(peer);
    if (idx != -1)
        m_dialogs->removeAt(idx);

    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);

    long long id = ((long long)peer->type() << 32) | peer->id();
    query.prepare("DELETE FROM dialogs WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    if (peer->type() != TGL_PEER_USER)
    {
        query.prepare("DELETE FROM peers WHERE id=:id");
        query.bindValue(":id", id);
        query.exec();
    }
}

struct photo_data
{
    long long photo_id;
    Peer* peer;
};

void Storage::_loadPhotoCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    photo_data* data = (photo_data*)callback_extra;
    m_instance->_PhotoLoaded();
    if (!success)
    {
        delete data;
        return;
    }

    Peer* peer = data->peer;
    peer->setPhoto(filename);
    peer->setPhotoId(data->photo_id);
    m_instance->markPeerDirty(peer);
    delete data;
}

void Storage::AsyncPhotoLoad(Peer* peer)
{
    if (!m_photosToLoad.contains(peer))
        m_photosToLoad.append(peer);

    _LoadNextPhoto();
}

void Storage::_LoadNextPhoto()
{
    if (m_photosToLoad.isEmpty() || m_photosInQuoue >= MAX_PHOTOS_TO_LOAD)
        return;

    m_photosInQuoue++;

    Peer* peer = m_photosToLoad.first();
    m_photosToLoad.removeFirst();

    switch(peer->type())
    {
        case TGL_PEER_USER:
            tgl_do_get_user_info(gTLS, {TGL_PEER_USER, peer->id()}, 0, _updateContactPhoto, NULL);
            break;
        case TGL_PEER_CHAT:
            tgl_do_get_chat_info(gTLS, {TGL_PEER_CHAT, peer->id()}, 0, _updateGroupPhoto, NULL);
            break;
    }
}

void Storage::_PhotoLoaded()
{
    m_photosInQuoue--;
    _LoadNextPhoto();
}

void Storage::userUpdateHandler(struct tgl_state *TLS, struct tgl_user *U, unsigned flags)
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
    CHECK_FLAG(flags, TGL_UPDATE_WORKING)
    CHECK_FLAG(flags, TGL_UPDATE_FLAGS)
    CHECK_FLAG(flags, TGL_UPDATE_TITLE)
    CHECK_FLAG(flags, TGL_UPDATE_ADMIN)
    CHECK_FLAG(flags, TGL_UPDATE_MEMBERS)
    CHECK_FLAG(flags, TGL_UPDATE_ACCESS_HASH)
    CHECK_FLAG(flags, TGL_UPDATE_USERNAME)
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

    /*if (flags & TGL_UPDATE_CREATED)
        m_instance->addContact(user);*/

    if (flags & TGL_UPDATE_PHONE)
        user->setPhone(QString::fromUtf8(U->phone));

    if (flags & TGL_UPDATE_NAME)
        user->setName(QString::fromUtf8(U->first_name), QString::fromUtf8(U->last_name));

    if (flags & TGL_UPDATE_PHOTO)
    {
        long long newPhotoId = (long long)U->photo_big.local_id << 32 | U->photo_small.local_id;
        if (user->getPhotoId() != newPhotoId)
            m_instance->AsyncPhotoLoad(user);
            //tgl_do_get_user_info(gTLS, {user->type(), user->id()}, 0, _updateContactPhoto, NULL);

        if ((flags & TGL_UPDATE_PHOTO) == flags)
            return;
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
        //todo somthing like async photo loader?
        //tgl_do_get_user_info(gTLS, peer, false, NULL, NULL);
    }

    m_instance->markPeerDirty(user);
}

void Storage::saveUpdatesToDatabase()
{
    QSqlDatabase &db = m_instance->m_db;

    if (m_updatedPeers.size() > 0)
    {
        db.transaction();
        QSqlQuery query(db);
        query.prepare("REPLACE INTO peers(id, data) VALUES(:id, :data)");

        for (int i = 0; i < m_updatedPeers.size(); i++)
        {
            Peer* peer = m_updatedPeers.value(i);
            long long peerId = ((long long)peer->type() << 32) | peer->id();
            query.bindValue(":id", peerId);
            QByteArray data = peer->serialize();
            query.bindValue(":data", data);
            query.exec();
        }
        m_updatedPeers.clear();
        db.commit();
    }
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

    if (peer_type == 0 || peer_id == 0)
        return;

    if (M->to_id.type == TGL_PEER_USER && M->to_id.id == gTLS->our_id)
    {
        peer_id = M->from_id.id;    // in
        User* user = (User*)m_instance->getPeer(TGL_PEER_USER, peer_id);
        if (user)
            user->resetTypingStatus();
    }
    if (M->from_id.type == TGL_PEER_USER && M->from_id.id == gTLS->our_id)
        peer_id = M->to_id.id;      // out

    Peer* peer = m_instance->getPeer(peer_type, peer_id);
    Message* message = m_instance->getMessage(M->id);

    GroupDataModel* messages = peer->m_messages;
    if (!messages->findExact(message).isEmpty())
        return;

    Message* lastMessage = peer->lastMessage();
    if ((!lastMessage || peer->lastMessage()->dateTime() < message->dateTime()) && message->from()->id() != TLS->our_id && !message->service())
    {
        emit m_instance->newMessageReceived(message);
    }

    messages->insert(message);

    if (m_instance->m_dialogs->indexOf(peer) == -1)
        m_instance->m_dialogs->append(peer);
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
        CHECK_FLAG(flags, TGL_UPDATE_WORKING)
        CHECK_FLAG(flags, TGL_UPDATE_FLAGS)
        CHECK_FLAG(flags, TGL_UPDATE_TITLE)
        CHECK_FLAG(flags, TGL_UPDATE_ADMIN)
        CHECK_FLAG(flags, TGL_UPDATE_MEMBERS)
        CHECK_FLAG(flags, TGL_UPDATE_ACCESS_HASH)
        CHECK_FLAG(flags, TGL_UPDATE_USERNAME)
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

    if (flags & TGL_UPDATE_CREATED)
        //todo somthing like async photo loader?
        //tgl_do_get_chat_info(gTLS, {groupChat->type(), groupChat->id()}, 0, NULL, NULL);


    if (flags & TGL_UPDATE_TITLE)
        groupChat->setTitle(QString::fromUtf8(C->title));

    if (flags & TGL_UPDATE_PHOTO)
    {
        long long newPhotoId = (long long)C->photo_big.local_id << 32 | C->photo_small.local_id;
        if (groupChat->getPhotoId() != newPhotoId)
            m_instance->AsyncPhotoLoad(groupChat);
            //tgl_do_get_chat_info(gTLS, {groupChat->type(), groupChat->id()}, 0, _updateGroupPhoto, NULL);

        if ((flags & TGL_UPDATE_PHOTO) == flags)
            return;
    }

    if (flags & TGL_UPDATE_ADMIN)
    {
        groupChat->setAdmin(C->admin_id);
    }

    if (flags & TGL_UPDATE_MEMBERS)
    {
        groupChat->setMembers(C->user_list, C->user_list_size);
    }

    m_instance->markPeerDirty(groupChat);
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

void Storage::messagesDeletedHandler(struct tgl_state *TLS, int num, int list[])
{
    for (int i = 0; i < num; i++)
        _deleteMessageCallback(TLS, (void*)list[i], 1);
}

void Storage::notifySettingsUpdateHandler(struct tgl_state *TLS, struct tgl_notify_peer_t *notify_peer, int mute_until, char* sound, int show_previews, int events_masks)
{
    if (notify_peer->type == tgl_notify_peer)
    {
        Peer* peer = m_instance->getPeer(notify_peer->peer.type, notify_peer->peer.id);
        peer->updateNotifySettings(mute_until, sound, show_previews, events_masks);
        m_instance->markPeerDirty(peer);
    }
    else
    {
        QMap<long long, Peer*>::iterator it;
        for (it = m_instance->m_peers.begin(); it != m_instance->m_peers.end(); ++it)
        {
            Peer* peer = it.value();
            if (notify_peer->type == tgl_notify_all ||
                    notify_peer->type == tgl_notify_users && peer->type() == TGL_PEER_USER ||
                    notify_peer->type == tgl_notify_chats && peer->type() != TGL_PEER_USER)
            {
                peer->updateNotifySettings(mute_until, sound, show_previews, events_masks);
            }
            m_instance->markPeerDirty(peer);
        }
    }
}

void Storage::encrChatUpdate(struct tgl_state *TLS, struct tgl_secret_chat *C, unsigned flags)
{
    QString str;
#define CHECK_FLAG(FLAGS,F) if ((FLAGS & F) == F) str += " " #F;

    CHECK_FLAG(flags, TGL_UPDATE_CREATED)
    CHECK_FLAG(flags, TGL_UPDATE_DELETED)
    CHECK_FLAG(flags, TGL_UPDATE_PHONE)
    CHECK_FLAG(flags, TGL_UPDATE_CONTACT)
    CHECK_FLAG(flags, TGL_UPDATE_PHOTO)
    CHECK_FLAG(flags, TGL_UPDATE_BLOCKED)
    CHECK_FLAG(flags, TGL_UPDATE_REAL_NAME)
    CHECK_FLAG(flags, TGL_UPDATE_WORKING)
    CHECK_FLAG(flags, TGL_UPDATE_FLAGS)
    CHECK_FLAG(flags, TGL_UPDATE_TITLE)
    CHECK_FLAG(flags, TGL_UPDATE_ADMIN)
    CHECK_FLAG(flags, TGL_UPDATE_MEMBERS)
    CHECK_FLAG(flags, TGL_UPDATE_ACCESS_HASH)
    CHECK_FLAG(flags, TGL_UPDATE_USERNAME)
    CHECK_FLAG(flags, TGL_UPDATE_REQUESTED)
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

    qDebug() << "Storage::encrChatUpdate user=" << QString::fromUtf8(C->print_name) << " flags=" << flags << str;


    if (flags & TGL_UPDATE_REQUESTED)
    {
        tgl_do_accept_encr_chat_request(gTLS, C, NULL, NULL);
    }


}

PeerDataModel* Storage::contacts() const
{
    return m_contacts;
}

PeerDataModel* Storage::dialogs() const
{
    return m_dialogs;
}


void Storage::_getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[])
{
    if (!success)
        return;

    Storage* _this = (Storage*)callback_extra;

    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);

    QList<long long> dialogs;
    query.exec("SELECT id FROM dialogs");
    while (query.next())
        dialogs.append(query.value(0).toLongLong());

    db.transaction();
    query.prepare("REPLACE INTO dialogs(id) VALUES(:id)");
    for (int i = 0; i < size; i++)
    {
        tgl_peer_id_t _peer = peers[i];
        long long peerId = ((long long)_peer.type << 32) | _peer.id;

        int idx = dialogs.indexOf(peerId);
        if (idx == -1)
        {
            query.bindValue(":id", peerId);
            query.exec();
        }
        else
            dialogs.removeAt(idx);

        if (_peer.type == TGL_PEER_USER)
            tgl_do_get_user_info(gTLS, _peer, 0, 0, 0);
        else if (_peer.type == TGL_PEER_CHAT)
            tgl_do_get_chat_info(gTLS, _peer, 0, 0, 0);


        Peer* peer = m_instance->getPeer(peers[i].type, peers[i].id);
        int last_chat_msg_id = peer->lastMessage() ? peer->lastMessage()->id() : 0;

        Message* lastMessage = 0;
        if (last_chat_msg_id != last_msg_id[i])
        {
            for (int j = peer->m_lapseMarkers.size() - 1; j >= 0; j--)
            {
                if (peer->m_lapseMarkers.at(j) >= last_msg_id[i])
                {
                    peer->m_lapseMarkers.removeAt(j);
                    continue;
                }
            }

            peer->m_lapseMarkers.push_back(last_msg_id[i]);

            lastMessage = m_instance->getMessage(last_msg_id[i]);
            peer->addMessage(lastMessage);

            tgl_do_get_history_maxid(gTLS, {peer->type(), peer->id()}, -1, last_msg_id[i], HISTORY_LIMIT, _getHistoryCallback, peer);
        }

        idx = 0;
        for (int i = 0; i < m_instance->m_dialogs->size(); i++)
        {
            Peer* p = m_instance->m_dialogs->value(i);
            if (p == peer)
            {
                idx = -1;
                break;
            }
            Message* last_msg = p->lastMessage();
            if (!lastMessage || (last_msg && last_msg->dateTime() > lastMessage->dateTime()))
                idx++;
        }
        if (idx != -1)
            m_instance->m_dialogs->insert(idx, peer);

    }
    if (dialogs.count() > 0)
    {
        for (int i = 0; i < dialogs.count(); i++)
        {
            long long peerId = dialogs[i];
            query.prepare("DELETE FROM dialogs WHERE id = :id");
            query.bindValue(":id", peerId);
            query.exec();

            if ((peerId >> 32) != TGL_PEER_USER)
            {
                query.prepare("DELETE FROM peers WHERE id=:id");
                query.bindValue(":id", peerId);
                query.exec();
            }
        }
    }
    db.commit();
}

void Storage::_getHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[])
{
    Peer* peer = (Peer*)callback_extra;

    if (success)
    {
        bool insert_marker = size == HISTORY_LIMIT;


        int begin = size == HISTORY_LIMIT ? list[size-1]->id : 0;
        int end = list[0]->id;

        QList<int>& markers = peer->m_lapseMarkers;
        for (int i = markers.size() - 1; i >= 0; i--)
            if (markers[i] >= begin && markers[i] <= end)
                markers.removeAt(i);

        QSqlDatabase &db = m_instance->m_db;
        db.transaction();

        GroupDataModel* messages = peer->m_messages;
        for (int i = 0; i < size; i++)
        {
            Message* message = m_instance->getMessage(list[i]->id);
            QVariantList idx = messages->findExact(message);
            if (!idx.isEmpty())
            {
                if (i == size - 1)
                    insert_marker = false;
                continue;
            }
            messages->insert(message);
            idx = messages->findExact(message);
        }
        db.commit();

        if (insert_marker)
        {
            int idx = 0;
            for (idx = 0; idx < markers.size(); idx++)
                if (markers[idx] > begin)
                    break;
            markers.insert(idx, begin);
        }
    }

    m_instance->markPeerDirty(peer);
    peer->m_loadingHistory = false;
}

void Storage::_deleteMessageCallback(struct tgl_state *TLS, void *callback_extra, int success)
{
    long long id = (int)callback_extra;

    Message* msg = m_instance->getMessage(id);

    for (int i = 0; i < m_instance->m_dialogs->size(); i++)
        m_instance->m_dialogs->value(i)->deleteMessage(msg);

    m_instance->m_messages.remove(id);
    delete msg;

    QSqlDatabase &db = m_instance->m_db;
    QSqlQuery query(db);
    query.prepare("DELETE FROM messages WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
}

void Storage::_deleteHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int offset)
{
    if (success)
    {
        Peer* peer = (Peer*)callback_extra;

        QSqlDatabase &db = m_instance->m_db;
        db.transaction();
        QSqlQuery query(db);
        query.prepare("DELETE FROM messages WHERE to_id=:to_id");
        query.bindValue(":to_id", peer->id());
        query.exec();

        bb::cascades::GroupDataModel* messages = (bb::cascades::GroupDataModel*)peer->messages();

        for (QVariantList indexPath = messages->first(); !indexPath.isEmpty(); indexPath = messages->after(indexPath))
        {
            QVariant item = messages->data(indexPath);
            Message* msg = (Message*)item.value<QObject*>();
            m_instance->m_messages.remove(msg->id());

            msg->setParent(0);//delete analogue
        }
        messages->clear();
        db.commit();
    }
}

void Storage::_updateContactsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *contacts[])
{
    QSqlDatabase &db = m_instance->m_db;
    db.transaction();
    QSqlQuery query(db);
    query.prepare("REPLACE INTO contacts(user_id) VALUES(:id)");
    for (int i = 0; i < size; i++)
    {
        query.bindValue(":id", contacts[i]->id.id);
        query.exec();
    }
    db.commit();
}

void Storage::_updateGroupPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C)
{
    if (!success)
    {
        m_instance->_PhotoLoaded();
        return;
    }

    GroupChat* groupChat = (GroupChat*)m_instance->getPeer(TGL_PEER_CHAT, C->id.id);

    long long newPhotoId = (long long)C->photo_big.local_id << 32 | C->photo_small.local_id;
    if (groupChat->getPhotoId() != newPhotoId)
    {
        if (C->photo.sizes_num != 0)
        {
            int min = 0;
            for (int i = 0; i < C->photo.sizes_num; i++)
            {
                if (C->photo.sizes[i].h + C->photo.sizes[i].w < C->photo.sizes[min].h + C->photo.sizes[min].w)
                    min = i;
            }
            photo_data* data = new photo_data;
            data->peer = groupChat;
            data->photo_id = newPhotoId;
            tgl_do_load_photo_size(gTLS, &C->photo.sizes[min], Storage::_loadPhotoCallback, data);
            //tgl_do_load_photo(gTLS, &C->photo, Storage::_loadPhotoCallback, groupChat);
        }
        else
        {
            m_instance->_PhotoLoaded();
            groupChat->setPhoto("");
        }
    }
    else
        m_instance->_PhotoLoaded();

    m_instance->markPeerDirty(groupChat);
}

void Storage::_updateContactPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *U)
{
    if (!success)
    {
        m_instance->_PhotoLoaded();
        return;
    }

    User* user = (User*)m_instance->getPeer(TGL_PEER_USER, U->id.id);

    long long newPhotoId = (long long)U->photo_big.local_id << 32 | U->photo_small.local_id;
    if (user->getPhotoId() != newPhotoId)
    {
        if (U->photo.sizes_num != 0)
        {
            int min = 0;
            for (int i = 0; i < U->photo.sizes_num; i++)
            {
                if (U->photo.sizes[i].h + U->photo.sizes[i].w < U->photo.sizes[min].h + U->photo.sizes[min].w)
                    min = i;
            }
            photo_data* data = new photo_data;
            data->peer = user;
            data->photo_id = newPhotoId;
            tgl_do_load_photo_size(gTLS, &U->photo.sizes[min], Storage::_loadPhotoCallback, data);
            //tgl_do_load_photo(gTLS, &U->photo, Storage::_loadPhotoCallback, user);
        }
        else
        {
            m_instance->_PhotoLoaded();
            user->setPhoto("");
        }
    }
    else
        m_instance->_PhotoLoaded();

    m_instance->markPeerDirty(user);
}

void Storage::_searchMessageCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[])
{
    if (success && size)
    {
    }
}

void Storage::updateChats()
{
    tgl_do_get_dialog_list(gTLS, _getDialogsCallback, this);
}

void Storage::updateContacts()
{
    tgl_do_update_contact_list(gTLS, _updateContactsCallback, NULL);
}

void Storage::updateUserInfo()
{
    tgl_peer_id_t userId = {TGL_PEER_USER, gTLS->our_id};
    tgl_do_get_user_info(gTLS, userId, false, NULL, NULL);
}

void Storage::loadAdditionalHistory(Peer* peer)
{
    int marker = 0;
    if (peer->m_lapseMarkers.size() > 0)
        marker = peer->m_lapseMarkers.last();

    GroupDataModel* messages = peer->m_messages;
    QVariantList idx;
    idx << messages->childCount(idx) - 1;
    idx << messages->childCount(idx) - 1;
    Message* msg = (Message*)messages->data(idx).value<QObject*>();
    int msg_id = msg->id();

    QSqlQuery query(m_db);
    query.prepare("SELECT id, from_id, to_id, to_type, text, date, data FROM messages WHERE ((to_id = :r_id AND to_type = :r_type) OR (to_id = :my_id AND to_type = 1 AND from_id = :s_id)) AND id > :marker AND id < :max_id ORDER BY date DESC LIMIT " + QString::number(HISTORY_LIMIT));

    query.bindValue(":r_type", peer->type());
    query.bindValue(":r_id", peer->id());
    query.bindValue(":my_id", gTLS->our_id);
    query.bindValue(":s_id", peer->id());
    query.bindValue(":marker", marker);
    query.bindValue(":max_id", msg_id);
    query.exec();
    int n = 0;
    while (query.next())
    {
        long long id = query.value(0).toLongLong();
        Message* message = getMessage(id);
        if (message == 0)
        {
            message = new Message(id);
            message->setParent(this);
            message->m_fromId = query.value(1).toInt();
            message->m_toId = query.value(2).toInt();
            message->m_toType = query.value(3).toInt();
            message->m_text = query.value(4).toString();
            message->m_date = QDateTime::fromTime_t(query.value(5).toInt());
            QByteArray data = query.value(6).toByteArray();
            message->deserialize(data);
            m_messages.insert(id, message);
        }
        if (peer->m_messages->findExact(message).isEmpty())
            peer->addMessage(message);
        n++;
    }

    if (peer->type() == TGL_BROADCAST_CHAT)
            return;

    if (n < HISTORY_LIMIT)
        tgl_do_get_history_maxid(gTLS, {peer->type(), peer->id()}, -1, msg_id, HISTORY_LIMIT, _getHistoryCallback, peer);
    else
        peer->m_loadingHistory = false;
}

void Storage::searchMessage(Peer* peer, int from, int to, int limit, int offset, const char *s)
{
    tgl_do_msg_search(gTLS, {peer->type(), peer->id()}, from, to, limit, offset, s, Storage::_searchMessageCallback, NULL);
}
