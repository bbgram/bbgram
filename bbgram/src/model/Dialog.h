#pragma once

#include "Chat.h"
#include "User.h"

class Dialog : public Chat
{
    Q_OBJECT
public:
    Dialog(User* user = 0);
    ~Dialog();

    QString title() const;
    QString status() const;
    QVariant photo() const;
protected:
    User*               m_user;
    QString             m_title;
    QString             m_status;
};

Q_DECLARE_METATYPE(Dialog*);
