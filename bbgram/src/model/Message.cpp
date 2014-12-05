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
    m_mediaType = M->media.type;
    if (m_mediaType != tgl_message_media_none)
    {
        if (m_mediaType == tgl_message_media_photo)
        {
            int max = -1;
            int maxi = -1;
            for (int i = 0; i < M->media.photo.sizes_num; i++)
            {
                  if (M->media.photo.sizes[i].w + M->media.photo.sizes[i].h > max)
                  {
                    max = M->media.photo.sizes[i].w + M->media.photo.sizes[i].h;
                    maxi = i;
                  }
            }
            tgl_photo_size &sz = M->media.photo.sizes[maxi];
            m_media.insert("width", sz.w);
            m_media.insert("height", sz.h);
            m_media.insert("size", sz.size);
            m_media.insert("volume", sz.loc.volume);
            m_media.insert("dc", sz.loc.dc);
            m_media.insert("local_id", sz.loc.local_id);
            m_media.insert("secret", sz.loc.secret);
        }
        else if (m_mediaType == tgl_message_media_contact)
        {
            m_media.insert("phone", QString::fromUtf8(M->media.phone));
            m_media.insert("first_name", QString::fromUtf8(M->media.first_name));
            m_media.insert("last_name", QString::fromUtf8(M->media.last_name));
            m_media.insert("user_id", M->media.user_id);
        }
        else if (m_mediaType == tgl_message_media_document)
        {
            int a = 1;
        }
       // tgl_do_load_photo()

        // @todo: put media
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
    {
        switch (m_mediaType)
        {
            case tgl_message_media_photo:
                return "photo";
            case tgl_message_media_video:
                return "#Unsupported media: video";
            case tgl_message_media_audio:
                return "#Unsupported media: audio";
            case tgl_message_media_document:
                return "#Unsupported media: document";
            case tgl_message_media_geo:
                return "#Unsupported media: geo";
            case tgl_message_media_contact:
                return "contact";
            case tgl_message_media_photo_encr:
                return "#Unsupported media: photo encr";
            case tgl_message_media_video_encr:
                return "#Unsupported media: video encr";
            case tgl_message_media_audio_encr:
                return "#Unsupported media: audio encr";
            case tgl_message_media_document_encr:
                return "#Unsupported media: document encr";
        }
    }
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

int Message::mediaType() const
{
    return m_mediaType;
}

QVariantMap& Message::media()
{
    return m_media;
}

User* Message::from() const
{
    return (User*)Storage::instance()->getPeer(TGL_PEER_USER, m_fromId);
}
