#pragma once

#include <QDateTime>
#include <QtCore/QUrl>

class Message;

class Wallpaper: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(int color READ color CONSTANT)
    Q_PROPERTY(bool isSolid READ isSolid CONSTANT)
    Q_PROPERTY(int backgroundColor READ backgroundColor CONSTANT)
    Q_PROPERTY(QUrl thumbnail READ thumbnail NOTIFY thumbnailChanged)
public:
    Wallpaper(tgl_wallpaper* W = 0);
    ~Wallpaper();

    int id() const;
    QString title() const;
    int color() const;
    bool isSolid() const;
    int backgroundColor() const;
    QUrl thumbnail() const;

    Q_INVOKABLE void load();

    static QString formatSize(int size);
signals:
    void thumbnailChanged();
    void loaded(const QUrl& url);
protected:
    static void _loadThumbnailCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename);
    static void _loadCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename);

    int     m_id;
    QString m_title;
    int     m_color;
    bool    m_isSolid;
    int     m_backgroundColor;
    QVariantList m_sizes;
    QUrl    m_thumbnail;
};

Q_DECLARE_METATYPE(Wallpaper*);
