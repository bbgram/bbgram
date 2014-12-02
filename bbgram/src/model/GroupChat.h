#pragma once

#include "Chat.h"

#include <bb/cascades/Image>

class GroupChat : public Chat
{
    Q_OBJECT
public:
    GroupChat(int id = 0);
    ~GroupChat();

    void update(tgl_chat *chat);

    QString title() const;
    QString status() const;
    QVariant photo() const;
protected:
    bb::cascades::Image     m_photo;
    QString                 m_title;
};

Q_DECLARE_METATYPE(GroupChat*);
