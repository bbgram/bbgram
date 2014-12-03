#pragma once

#include <QDateTime>

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
public:
    Message(long long id = 0, tgl_message* M = 0);
    ~Message();

    long long id() const;
    bool our() const;

    const QString& text() const;
    const QDate& date() const;
    const QDateTime& dateTime() const;
    QString dateFormatted() const;

    bool unread() const;
    void markAsRead();
signals:
    void markedRead();
protected:
    long long   m_id;
    int         m_from_id;

    QString     m_text;
    QDateTime   m_date;
    bool        m_unread;
};

Q_DECLARE_METATYPE(Message*);
