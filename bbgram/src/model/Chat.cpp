#include "Chat.h"

Chat::Chat()
{
}

Chat::~Chat()
{
}

QString Chat::title() const
{
    return QString();
}

QString Chat::status() const
{
    return QString();
}

QVariant Chat::photo() const
{
    return QVariant();
}

Message* Chat::lastMessage() const
{
    return m_messages.size() > 0 ? m_messages.last() : 0;
}

void Chat::addMessage(Message* message)
{
    m_messages.append(message);
    emit messagesChanged();
}

