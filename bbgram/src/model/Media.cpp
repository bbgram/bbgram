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
        if (m_message->mediaType() == 1)
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

void Media::_loadThumbnailCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    Media* photo = (Media*)callback_extra;
    photo->m_thumbnail = QUrl::fromLocalFile(filename);
    emit photo->thumbnailChanged();
}
