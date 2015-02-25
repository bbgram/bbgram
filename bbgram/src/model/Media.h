#pragma once

#include <QDateTime>
#include <QtCore/QUrl>

class Message;

class Media: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString caption READ caption CONSTANT)
    Q_PROPERTY(QUrl thumbnail READ thumbnail NOTIFY thumbnailChanged)
public:
    Media(Message* message = 0);
    ~Media();

    Message* message() const;
    QString caption() const;
    QUrl thumbnail() const;
signals:
    void thumbnailChanged();
protected:
    static void _loadThumbnailCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename);

    friend class Storage;
    Message*    m_message;
    QUrl        m_thumbnail;
};

Q_DECLARE_METATYPE(Media*);
