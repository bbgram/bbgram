#include "EncrChat.h"
#include "../utils/Colorizer.h"

extern "C"
{
    #include <tgl-binlog.h>
}
#include <assert.h>

using namespace bb::cascades;

EncrChat::EncrChat(int id)
    : Peer(TGL_PEER_ENCR_CHAT, id), m_secret_chat(NULL), m_companion(NULL)//, m_online(false), m_typingStatus(tgl_typing_none)
{
    /*connect(this, SIGNAL(titleChanged()), this, SIGNAL(nameChanged()));
    connect(this, SIGNAL(typingStatusChanged()), this, SIGNAL(statusChanged()));
    connect(this, SIGNAL(messagesChanged()), this, SIGNAL(statusChanged()));*/
}

EncrChat::~EncrChat()
{
}

void EncrChat::load(const QVariantMap& map)
{
    Peer::load(map);

    QVariantMap::const_iterator it;

    it = map.find("encrChatSettings");
    if (it != map.end())
    {
        QVariantMap encrChatSettings = it.value().toMap();

        QString print_name;
        int user_id, admin_id, date, ttl, layer, state, in_seq_no, out_seq_no, last_in_seq_no;
        long long access_hash, key_fingerprint;

        QByteArray key, first_key_sha;

        it = encrChatSettings.find("user_id");
        user_id = it.value().toInt();

        it = encrChatSettings.find("admin_id");
        admin_id = it.value().toInt();

        it = encrChatSettings.find("date");
        date = it.value().toInt();

        it = encrChatSettings.find("ttl");
        ttl = it.value().toInt();

        it = encrChatSettings.find("layer");
        layer = it.value().toInt();


        it = encrChatSettings.find("access_hash");
        access_hash = it.value().toLongLong();

        it = encrChatSettings.find("state");
        state = it.value().toInt();

        it = encrChatSettings.find("key_fingerprint");
        key_fingerprint = it.value().toLongLong();

        it = encrChatSettings.find("key");
        key = it.value().toByteArray();

        it = encrChatSettings.find("first_key_sha");
        first_key_sha = it.value().toByteArray();

        it = encrChatSettings.find("last_in_seq_no");
        last_in_seq_no = it.value().toInt();

        it = encrChatSettings.find("in_seq_no");
        in_seq_no = it.value().toInt();

        it = encrChatSettings.find("out_seq_no");
        out_seq_no = it.value().toInt();

        bl_do_encr_chat_create (gTLS, id(), user_id, admin_id, print_name.toUtf8().data(), print_name.toUtf8().size());
        tgl_secret_chat* secret_chat = (tgl_secret_chat *)tgl_peer_get (gTLS, TGL_MK_ENCR_CHAT(id()));

        assert(secret_chat && (secret_chat->flags & FLAG_CREATED));
        bl_do_encr_chat_set_date(gTLS, secret_chat, date);
        bl_do_encr_chat_set_ttl(gTLS, secret_chat, ttl);
        bl_do_encr_chat_set_layer(gTLS ,secret_chat, layer);
        bl_do_encr_chat_set_access_hash(gTLS, secret_chat, access_hash);
        bl_do_encr_chat_set_state(gTLS, secret_chat, (tgl_secret_chat_state)state);
        bl_do_encr_chat_set_key(gTLS, secret_chat, (unsigned char*)key.data(), key_fingerprint);
        bl_do_encr_chat_set_sha(gTLS, secret_chat, (unsigned char*)first_key_sha.data());
        bl_do_encr_chat_set_seq(gTLS, secret_chat, in_seq_no, last_in_seq_no, out_seq_no);

        m_secret_chat = secret_chat;
    }
}

void EncrChat::save(QVariantMap& map) const
{
    Peer::save(map);

    if (m_secret_chat)
    {
        QVariantMap encrChatSettings;
        encrChatSettings["print_name"] = QString(m_secret_chat->print_name);
        encrChatSettings["user_id"] = m_secret_chat->user_id;
        encrChatSettings["admin_id"] = m_secret_chat->admin_id;
        encrChatSettings["date"] = m_secret_chat->date;
        encrChatSettings["ttl"] = m_secret_chat->ttl;
        encrChatSettings["layer"] = m_secret_chat->layer;
        encrChatSettings["access_hash"] = m_secret_chat->access_hash;
        encrChatSettings["state"] = (int)m_secret_chat->state;
        encrChatSettings["key_fingerprint"] = m_secret_chat->key_fingerprint;

        QByteArray key((char*)m_secret_chat->key, 256);
        encrChatSettings["key"] = key;

        QByteArray first_key_sha((char*)m_secret_chat->first_key_sha, 20);
        encrChatSettings["first_key_sha"] = first_key_sha;

        encrChatSettings["in_seq_no"] = m_secret_chat->in_seq_no;
        encrChatSettings["last_in_seq_no"] = m_secret_chat->last_in_seq_no;
        encrChatSettings["in_seq_no"] = m_secret_chat->in_seq_no;
        encrChatSettings["out_seq_no"] = m_secret_chat->out_seq_no;

        map.insert("encrChatSettings", encrChatSettings);
    }
}

QString EncrChat::title() const
{
    if (m_companion)
        return QString("Encr: ") + m_companion->title();
    else
        return QString("Encr: ");
}

QString EncrChat::status() const
{
    if (lastMessage())
        return "<p>" + lastMessage()->text() + "</p>";
    return "";
}

void EncrChat::setCompanion(User* user)
{
    m_companion = user;
    setPhoto(user->photoFileName());

    emit titleChanged();
    emit statusChanged();
}

void EncrChat::setSecretInfo(tgl_secret_chat* info)
{
    m_secret_chat = info;
}
