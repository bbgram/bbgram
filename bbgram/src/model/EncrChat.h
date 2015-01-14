#pragma once

#include <QMap>
#include <bb/cascades/Image>

#include "Peer.h"

class EncrChat : public Peer
{
    Q_OBJECT
public:
    EncrChat(int id = 0);
    ~EncrChat();

    void load(const QVariantMap& map);
    void save(QVariantMap& map) const;
protected:
    QString     m_phone;
    QString     m_firstName;
    QString     m_lastName;

    bool        m_online;
    QDateTime   m_lastSeen;

    tgl_typing_status   m_typingStatus;
};

Q_DECLARE_METATYPE(EncrChat*);
