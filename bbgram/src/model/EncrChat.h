#pragma once

#include <QMap>
#include <bb/cascades/Image>

#include "Peer.h"
#include "User.h"

class EncrChat : public Peer
{
    Q_OBJECT
public:
    EncrChat(int id = 0);
    ~EncrChat();

    void load(const QVariantMap& map);
    void save(QVariantMap& map) const;

    QString title() const;
    QString status() const;

    void addMessage(Message* message);
    void setCompanion(User* user);
    void setSecretInfo(tgl_secret_chat* info);
protected:
    tgl_secret_chat* m_secret_chat;
    User* m_companion;

    tgl_typing_status   m_typingStatus;
};

Q_DECLARE_METATYPE(EncrChat*);
