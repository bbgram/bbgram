#pragma once

#include "Peer.h"
#include "User.h"
#include "GroupChat.h"

#include <bb/cascades/DataModel>
#include <bb/cascades/QListDataModel>

#define TGL_BROADCAST_CHAT 255

class BroadcastChat : public GroupChat
{
    Q_OBJECT
public:
    BroadcastChat(int id);
    ~BroadcastChat();

    void load(const QVariantMap& map);
    void save(QVariantMap& map) const;
protected:
};

Q_DECLARE_METATYPE(BroadcastChat*);
