#pragma once

#include <QDateTime>
class User;

class Message: public QObject
{
    Q_OBJECT

    Q_PROPERTY(long long id READ id CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(QDate date READ date CONSTANT)
    Q_PROPERTY(QDateTime dateTime READ dateTime CONSTANT)
    Q_PROPERTY(QString dateFormatted READ dateFormatted CONSTANT)
    Q_PROPERTY(bool our READ our CONSTANT)
    Q_PROPERTY(bool unread READ unread NOTIFY markedRead)
    Q_PROPERTY(bool service READ service CONSTANT)
    Q_PROPERTY(QVariantMap media READ media CONSTANT)
    Q_PROPERTY(int mediaType READ mediaType CONSTANT)
    Q_PROPERTY(User* from READ from CONSTANT)
    Q_PROPERTY(QDateTime forwardedDate READ forwardedDate CONSTANT)
    Q_PROPERTY(User* forwardedFrom READ forwardedFrom CONSTANT)
public:
    Message(long long id = 0, tgl_message* M = 0);
    ~Message();

    void deserialize(QByteArray& data);
    QByteArray serialize() const;

    long long id() const;
    bool our() const;

    QString text() const;
    const QDate& date() const;
    const QDateTime& dateTime() const;
    QString dateFormatted() const;

    bool unread() const;
    void markAsRead();
    bool service() const;
    int mediaType() const;
    QVariantMap& media();

    User* from() const;

    const QDateTime& forwardedDate() const;
    User* forwardedFrom() const;

    Q_INVOKABLE QString formatDateTime(const QDateTime& date, const QString& format);
signals:
    void markedRead();
protected:
    friend class Storage;

    long long   m_id;
    int         m_fromId;
    int         m_toId;
    int         m_toType;

    int         m_fwdFromId;
    QDateTime   m_fwdDate;

    QString     m_text;
    QDateTime   m_date;
    bool        m_unread;
    bool        m_service;
    QVariantMap m_action;
    int         m_mediaType;
    QVariantMap m_media;
};

Q_DECLARE_METATYPE(Message*);
