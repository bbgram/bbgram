#include "Storage.h"

#include "model/Dialog.h"

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
    m_chats = new QListDataModel<Chat*>();

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
    QMap<int, User*>::iterator it = m_users.find(id);
    if (it != m_users.end())
        return it.value();
    else
    {
        User* user = new User(id);
        m_users.insert(id, user);
        return user;
    }
}

User* Storage::findUser(int id)
{
    QMap<int, User*>::iterator it = m_users.find(id);
    if (it != m_users.end())
        return it.value();
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
            m_messages.insert(id, message);
            return message;
        }
        else
            return 0;
    }
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


QListDataModel<User*>* Storage::contacts() const
{
    return m_contacts;
}

QListDataModel<Chat*>* Storage::chats() const
{
    return m_chats;
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

    User* user = m_instance->findUser(U->id.id);
    Dialog* dialog = new Dialog(user);
    Message* lastMessage = 0;
    if (U->last)
    {
        lastMessage = m_instance->getMessage(U->last->id);
        if (lastMessage)
            dialog->addMessage(lastMessage);
    }
    int idx = 0;
    for (int i = 0; i < m_instance->m_chats->size(); i++)
    {
        Chat* chat = m_instance->m_chats->value(i);
        Message* chatLastMessage = chat->lastMessage();
        if (!lastMessage || (chatLastMessage && chatLastMessage->date() > lastMessage->date()))
            idx++;
    }
    m_instance->m_chats->insert(idx, dialog);
}

/*void get_chat_info_callback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C)
{
    if (!success)
        return;
}*/

void Storage::_getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[])
{
    if (!success)
        return;

    Storage* _this = (Storage*)callback_extra;

    _this->m_chats->clear();
    for (int i = 0; i < size; i++)
    {
        tgl_peer_id_t _peer = peers[i];
        int peer_type = peers[i].type;
        int _last_msg_id = last_msg_id[i];
        int _unread_count = unread_count[i];
        //tgl_do_get_history_ext(gTLS, _peer, _last_msg_id, 1, 0, get_history_callback, 0);

        if (peer_type == TGL_PEER_USER)
            tgl_do_get_user_info(gTLS, _peer, 0, _getUserInfoCallback, _this);
        /*else if (peer_type == TGL_PEER_CHAT)
            tgl_do_get_chat_info(gTLS, _peer, 0, get_chat_info_callback, 0);*/
    }
}

void Storage::updateContacts()
{
    tgl_do_update_contact_list(gTLS, _getContactsCallback, this);
}

void Storage::updateChats()
{
    tgl_do_get_dialog_list(gTLS, _getDialogsCallback, this);
}

