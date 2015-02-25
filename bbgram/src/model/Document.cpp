#include "Document.h"
#include "Message.h"
#include "../Storage.h"
#include <QLocale>

Document::Document(Message* message)
    : m_message(message)
{
}

Document::~Document()
{
}

Message* Document::message() const
{
    return m_message;
}

QString Document::caption() const
{
    return m_message->media().value("caption").toString();
}

QString Document::details() const
{
    QString size = formatSize(m_message->media().value("size").toInt());

    QLocale locale(QLocale::English);
    QString date = locale.toString(m_message->dateTime(), "dd.MM.yyyy") + " at " + locale.toString(m_message->dateTime(), "hh:mm");
    return QString("%1, %2").arg(size).arg(date);
}

QString Document::extension() const
{
    QString caption = m_message->media().value("caption").toString();
    QFileInfo fi(caption);
    return fi.suffix().toLower();
}

QUrl Document::thumbnail() const
{
    if (m_thumbnail.isEmpty())
    {
        QVariantMap& media = m_message->media();
        QVariantMap thumb = media.value("thumb").toMap();

        tgl_photo_size photoSize;
        photoSize.w = thumb["width"].toInt();
        photoSize.w = thumb["height"].toInt();
        photoSize.size = thumb["size"].toInt();
        photoSize.loc.volume = thumb["volume"].toLongLong();
        photoSize.loc.dc = thumb["dc"].toInt();
        photoSize.loc.local_id = thumb["local_id"].toInt();
        photoSize.loc.secret = thumb["secret"].toLongLong();

        tgl_do_load_photo_size(gTLS, &photoSize, _loadThumbnailCallback, (void*)this);
    }
    return m_thumbnail;
}

void Document::_loadThumbnailCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    Document* doc = (Document*)callback_extra;

    doc->m_thumbnail = QUrl::fromLocalFile(filename);
    emit doc->thumbnailChanged();
}

QString Document::formatSize(int size)
{
    const char* units[] = {"B", "KB", "MB", "GB"};

    float sz = size;
    int i = 0;
    while (sz > 1024) {
        sz /= 1024.f;
        i++;
    }
    return QString("%1 %2").arg(QString::number(qMax(sz, 0.1f), 'f', 1)).arg(QString::fromUtf8(units[i]));
}
