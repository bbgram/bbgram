#include "Message.h"
#include "../Storage.h"
#include "User.h"
#include <QLocale>

Message::Message(long long id, tgl_message* M)
    : m_id(id)
{
    if (M->service == 0 && M->message)
        m_text = QString::fromUtf8(M->message);
    m_date = QDateTime::fromTime_t(M->date);
    m_fromId = M->from_id.id;
    m_unread = M->unread;
    m_service = M->service;
    if (m_service)
    {
        m_action.insert("type", M->action.type);
        if (M->action.type == tgl_message_action_chat_edit_title)
            m_action.insert("title", QString::fromUtf8(M->action.new_title));
        if (M->action.type == tgl_message_action_chat_add_user || M->action.type == tgl_message_action_chat_delete_user)
            m_action.insert("user", M->action.user);
        if (M->action.type == tgl_message_action_chat_edit_photo)
        {
            // @todo: put photo
        }
    }
}

Message::~Message()
{
}

long long Message::id() const
{
    return m_id;
}

bool Message::our() const
{
    return m_fromId == gTLS->our_id;
}

const QDate& Message::date() const
{
    return m_date.date();
}

const QDateTime& Message::dateTime() const
{
    return m_date;
}

QString formatUser(int id)
{
    QString author="";
    if (id == gTLS->our_id)
        author = "You";
    else
    {
        User* user = (User*)Storage::instance()->getPeer(TGL_PEER_USER, id);
        if (user)
            author = user->firstName() + " " + user->lastName();
    }
    return author;
}

QString Message::text() const
{
    if (m_service)
    {
        QString text = "";
        int type = m_action["type"].toInt();
        QString user = formatUser(m_fromId);
        if (type == tgl_message_action_chat_create)
        {
            text = QString("%1 created the group").arg(user);
        }
        else if (type == tgl_message_action_chat_edit_title)
        {
            QString title = m_action["title"].toString();
            text = QString("%1 changed the group name to %2").arg(user, title);
        }
        else if (type == tgl_message_action_chat_edit_photo)
        {
            text = QString("%1 changed the group photo").arg(user);
        }
        else if (type == tgl_message_action_chat_delete_photo)
        {
            text = QString("%1 removed the group photo").arg(user);
        }
        else if (type == tgl_message_action_chat_add_user)
        {
            int userId = m_action["user"].toInt();
            if (userId == gTLS->our_id)
                text = QString("%1 added you").arg(user);
            else
                text = QString("%1 added %2").arg(user, formatUser(userId));
        }
        else if (type == tgl_message_action_chat_delete_user)
        {
            int userId = m_action["user"].toInt();
            if (userId == gTLS->our_id)
                text = QString("%1 removed you").arg(user);
            else
                text = QString("%1 removed %2").arg(user, formatUser(userId));
        }
        else
            text = QString("system message with type=%1").arg(type);

        return text;
    }
    else
        return m_text;
}

QString Message::dateFormatted() const
{
    QLocale locale(QLocale::English);
    QString str;
    QDate date = m_date.date();
    QDateTime currentDate = QDateTime::currentDateTime();
    if (m_date.secsTo(currentDate) < 24*60*60)
        str = locale.toString(m_date, "hh:mm");
    else
    {
        if (m_date.daysTo(currentDate) < 7)
            str = locale.toString(m_date, "ddd");
        else
            str = locale.toString(m_date, "MMM dd");
    }
    return str;
}

bool Message::unread() const
{
    return m_unread;
}

void Message::markAsRead()
{
    m_unread = 0;
    emit markedRead();
}

bool Message::service() const
{
    return m_service;
}
