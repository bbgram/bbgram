#include "Dialog.h"

Dialog::Dialog(User* user)
    : Chat(TGL_PEER_USER, user->id()), m_user(user)
{
    connect(m_user, SIGNAL(photoChanged()), this, SIGNAL(photoChanged()));

    m_title = m_user->firstName() + " " + m_user->lastName();
    emit titleChanged();

    connect(m_user, SIGNAL(typingStatusChanged()), this, SIGNAL(statusChanged()));
    connect(this, SIGNAL(messagesChanged()), this, SIGNAL(statusChanged()));
}

Dialog::~Dialog()
{
}

User* Dialog::user() const
{
    return m_user;
}

QString Dialog::title() const
{
    return m_title;
}

QString Dialog::status() const
{
    QString typingStatus = m_user->typingStatus();
    if (typingStatus.length() > 0)
        return "<p style='color:#236EBB'>" + typingStatus + "</p>";
    Message* message = lastMessage();
    if (message)
        return "<p>" + message->text() + "</p>";
}

QVariant Dialog::photo() const
{
    return m_user->photo();
}
