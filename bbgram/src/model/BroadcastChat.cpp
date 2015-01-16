#include "BroadcastChat.h"
#include "../utils/Colorizer.h"
#include "../Storage.h"

using namespace bb::cascades;

BroadcastChat::BroadcastChat(int id)
    : GroupChat(id)
{
    m_type = TGL_BROADCAST_CHAT;
}

BroadcastChat::~BroadcastChat()
{
}

void BroadcastChat::load(const QVariantMap& map)
{
    GroupChat::load(map);
}

void BroadcastChat::save(QVariantMap& map) const
{
    GroupChat::save(map);
}


