#include "Wallpaper.h"

Wallpaper::Wallpaper(tgl_wallpaper* W)
{
    if (!W)
        return;
    m_isSolid = W->solid != 0;
    m_id = W->id;
    m_title = QString::fromUtf8(W->title);
    m_color = W->color;
    m_backgroundColor = W->solid != 0 ? W->bg_color : 0;
    if (m_isSolid == 0)
    {
       for (int i = 0; i < W->sizes_num; i++)
       {
           tgl_photo_size& sz = W->sizes[i];

           QVariantMap size;
           size.insert("type", sz.type);
           size.insert("width", sz.w);
           size.insert("height", sz.h);
           size.insert("size", sz.size); // why sz.size == 0??
           size.insert("volume", sz.loc.volume);
           size.insert("dc", sz.loc.dc);
           size.insert("local_id", sz.loc.local_id);
           size.insert("secret", sz.loc.secret);

           m_sizes.append(size);
       }
    }
}

Wallpaper::~Wallpaper()
{
}

int Wallpaper::id() const
{
    return m_id;
}

QString Wallpaper::title() const
{
    return m_title;
}

int Wallpaper::color() const
{
    return m_backgroundColor;
}

bool Wallpaper::isSolid() const
{
    return m_isSolid;
}

int Wallpaper::backgroundColor() const
{
    return m_backgroundColor;
}

#include <qdebug.h>

QUrl Wallpaper::thumbnail() const
{
    if (m_thumbnail.isEmpty())
    {
        int h = 0;
        QVariantMap sz;
        for (int i = 0; i < m_sizes.size(); i++)
        {
            int _h = sz["height"].toInt();
            if (h == 0 || abs(_h - 250) < abs(h - 250))
            {
                sz = m_sizes[i].toMap();
                h = _h;
            }
        }
        tgl_photo_size photoSize;
        photoSize.w = sz["width"].toInt();
        photoSize.h = sz["height"].toInt();
        photoSize.size = sz["size"].toInt();
        photoSize.loc.volume = sz["volume"].toLongLong();
        photoSize.loc.dc = sz["dc"].toInt();
        photoSize.loc.local_id = sz["local_id"].toInt();
        photoSize.loc.secret = sz["secret"].toLongLong();
        tgl_do_load_photo_size(gTLS, &photoSize, _loadThumbnailCallback, (void*)this);
    }
    return m_thumbnail;
}

void Wallpaper::_loadThumbnailCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename)
{
    Q_UNUSED(TLS);
    Q_UNUSED(success);

    Wallpaper* wallpaper = (Wallpaper*)callback_extra;
    wallpaper->m_thumbnail = QUrl::fromLocalFile(filename);
    emit wallpaper->thumbnailChanged();
}

void Wallpaper::load()
{
    QVariantMap sz;
    int h = 0;
    for (int i = 0; i < m_sizes.size(); i++)
    {
        QVariantMap m = m_sizes[i].toMap();
        int _h = m["height"].toInt();
        if (h == 0 || abs(_h - 1280) < abs(h - 1280))
        {
            sz = m;
            h = _h;
        }
    }
    tgl_photo_size photoSize;
    photoSize.w = sz["width"].toInt();
    photoSize.h = sz["height"].toInt();
    photoSize.size = sz["size"].toInt();
    photoSize.loc.volume = sz["volume"].toLongLong();
    photoSize.loc.dc = sz["dc"].toInt();
    photoSize.loc.local_id = sz["local_id"].toInt();
    photoSize.loc.secret = sz["secret"].toLongLong();
    tgl_do_load_photo_size(gTLS, &photoSize, _loadCallback, (void*)this);
}

void Wallpaper::_loadCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename)
{
    Q_UNUSED(TLS);
    if (success)
    {
        Wallpaper* wallpaper = (Wallpaper*)callback_extra;
        emit wallpaper->loaded(QUrl::fromLocalFile(filename));
        wallpaper->disconnect();
    }
}
