#include "Media.h"
#include "Message.h"
#include "../Storage.h"
#include <QLocale>

Media::Media(Message* message)
    : m_message(message)
{
}

Media::~Media()
{
}

Message* Media::message() const
{
    return m_message;
}

QString Media::caption() const
{
    return m_message->media().value("caption").toString();
}

QUrl Media::thumbnail() const
{
    if (m_thumbnail.isEmpty())
    {
        //photo
        QVariantMap& media = m_message->media();

        QVariantMap sz;
        if (m_message->mediaType() == tgl_message_media_photo)
        {
            QVariantList sizes = media["sizes"].toList();
            for (int i = 0; i < sizes.size(); i++)
            {
                sz = sizes[i].toMap();
                if (sz["type"].toString() == "s")
                    break;
            }
        }
        else
        {
            sz = media["thumb"].toMap();
        }
        tgl_photo_size photoSize;
        photoSize.w = sz["width"].toInt();
        photoSize.w = sz["height"].toInt();
        photoSize.size = sz["size"].toInt();
        photoSize.loc.volume = sz["volume"].toLongLong();
        photoSize.loc.dc = sz["dc"].toInt();
        photoSize.loc.local_id = sz["local_id"].toInt();
        photoSize.loc.secret = sz["secret"].toLongLong();

        tgl_do_load_photo_size(gTLS, &photoSize, _loadThumbnailCallback, (void*)this);
    }
    return m_thumbnail;
}

int Media::flags() const
{
    if (m_message->mediaType() == tgl_message_media_photo)
        return FLAG_DOCUMENT_IMAGE;
    else
        return m_message->media().value("flags").toInt();
}

int Media::duration() const
{
    if (m_message->mediaType() == tgl_message_media_photo)
       return 0;
   else
       return m_message->media().value("duration").toInt();
}

QString Media::durationFormatted() const
{
    int t = duration();
    int h = t / 3600;
    int m = (t / 60) % 60;
    int s = t % 60;
    if (h != 0)
        return QString("%1:%2:%3").arg(h).arg(m, 2, 10, QLatin1Char('0')).arg(s, 2, 10, QLatin1Char('0'));
    else
        return QString("%1:%2").arg(m).arg(s, 2, 10, QLatin1Char('0'));
}

void Media::_loadThumbnailCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    Media* photo = (Media*)callback_extra;
    photo->m_thumbnail = QUrl::fromLocalFile(filename);
    emit photo->thumbnailChanged();
}
