#include "Chat.h"
#include "MessagesDataModel.h"

using namespace bb::cascades;

Chat::Chat(int type, int id)
    : m_type(type), m_id(id)
{
    m_messages = new MessagesDataModel(this);
    m_messages->setSortingKeys(QStringList() << "date" << "dateTime");
    m_messages->setSortedAscending(false);
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
    if (m_messages->size() > 0)
    {
        QVariant data = m_messages->data(QVariantList() << 0 << 0);
        return (Message*)data.value<QObject*>();
    }
    return 0;
}

QDateTime Chat::lastMessageDate() const
{
    Message* last = lastMessage();
    return last ? last->dateTime() : QDateTime();
}

void Chat::addMessage(Message* message)
{
    m_messages->insert(message);
}

void Chat::deleteMessage(Message* message)
{
    m_messages->remove(message);
}
