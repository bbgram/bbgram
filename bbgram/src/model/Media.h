#pragma once

#include <QDateTime>
#include <QtCore/QUrl>

class Message;

class Media: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString caption READ caption CONSTANT)
    Q_PROPERTY(QUrl thumbnail READ thumbnail NOTIFY thumbnailChanged)
    Q_PROPERTY(int flags READ flags CONSTANT)
    Q_PROPERTY(int duration READ duration CONSTANT)
    Q_PROPERTY(QString durationFormatted READ durationFormatted CONSTANT)
public:
    Media(Message* message = 0);
    ~Media();

    Message* message() const;
    QString caption() const;
    QUrl thumbnail() const;
    int flags() const;
    int duration() const;
    QString durationFormatted() const;
signals:
    void thumbnailChanged();
protected:
    static void _loadThumbnailCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename);

    friend class Storage;
    Message*    m_message;
    QUrl        m_thumbnail;
};

Q_DECLARE_METATYPE(Media*);
