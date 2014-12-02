#include "Chat.h"

using namespace bb::cascades;

Chat::Chat(int type, int id)
    : m_type(type), m_id(id)
{
    m_messages = new QListDataModel<Message*>();
    connect(m_messages, SIGNAL(itemAdded(QVariantList)), this, SIGNAL(messagesChanged()));
    connect(m_messages, SIGNAL(itemRemoved(QVariantList)), this, SIGNAL(messagesChanged()));
}

Chat::~Chat()
{
}

int Chat::type() const
{
    return m_type;
}

int Chat::id() const
{
    return m_id;
}

bb::cascades::DataModel* Chat::messages() const
{
    return m_messages;
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
    return m_messages->size() > 0 ? m_messages->value(0) : 0;
}

void Chat::addMessage(Message* message)
{
    m_messages->append(message);
}

void Chat::deleteMessage(Message* message)
{
    for (int i = 0; i < m_messages->size(); i++)
    {
        if (m_messages->value(i) == message)
        {
            m_messages->removeAt(i);
            break;
        }
    }
}
