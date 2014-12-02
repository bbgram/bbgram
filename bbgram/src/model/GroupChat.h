#pragma once

#include "Chat.h"

#include <bb/cascades/Image>

class GroupChat : public Chat
{
    Q_OBJECT
public:
    GroupChat(int id = 0);
    ~GroupChat();

    QString title() const;
    void setTitle(const QString& title);

    QString status() const;
    QVariant photo() const;
    void setPhoto(const QString &filename);
protected:
    bb::cascades::Image     m_photo;
    QString                 m_photoFilename;
    QString                 m_title;
};

Q_DECLARE_METATYPE(GroupChat*);
