#pragma once

#include "Chat.h"
#include "User.h"

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

    User* admin() const;
    void setAdmin(User* admin);

    void setMembers(const QList<User*>& members);
    const QList<User*>& getMembers() const;

signals:
        void adminChanged();
        void membersChanged();
protected:
    bb::cascades::Image     m_photo;
    QString                 m_photoFilename;
    QString                 m_title;
    User*                   m_admin;
    QList<User*>            m_members;
};

Q_DECLARE_METATYPE(GroupChat*);
