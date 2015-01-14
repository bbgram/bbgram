#include "EncrChat.h"
#include "../utils/Colorizer.h"

using namespace bb::cascades;

EncrChat::EncrChat(int id)
    : Peer(TGL_PEER_ENCR_CHAT, id)//, m_online(false), m_typingStatus(tgl_typing_none)
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

    /*it = map.find("phone");
    if (it != map.end())
        setPhone(it.value().toString());
    it = map.find("firstName");
    if (it != map.end())
        m_firstName = it.value().toString();
    it = map.find("lastName");
    if (it != map.end())
        m_lastName = it.value().toString();
    emit nameChanged();
    it = map.find("lastSeen");
    if (it != map.end())
        setStatus(0, QDateTime::fromTime_t(it.value().toUInt()));*/
}

void EncrChat::save(QVariantMap& map) const
{
    Peer::save(map);

    /*map.insert("phone", m_phone);
    map.insert("firstName", m_firstName);
    map.insert("lastName", m_lastName);
    map.insert("lastSeen", m_lastSeen.toTime_t());*/
}
