#pragma once

#include "Chat.h"
#include "User.h"

class Dialog : public Chat
{
    Q_OBJECT

    Q_PROPERTY(User* user READ user NOTIFY userChanged)
public:
    Dialog(User* user = 0);
    ~Dialog();

    User* user() const;

    QString title() const;
    QString status() const;
    QVariant photo() const;
signals:
    void userChanged();
protected:
    User*               m_user;
    QString             m_title;
};

Q_DECLARE_METATYPE(Dialog*);
