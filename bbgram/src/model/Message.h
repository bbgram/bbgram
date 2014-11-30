#pragma once

#include <QDateTime>

class Message: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(QDateTime date READ date CONSTANT)
    Q_PROPERTY(QString dateFormatted READ dateFormatted CONSTANT)
    Q_PROPERTY(bool our READ our CONSTANT)
public:
    Message(long long id = 0, tgl_message* M = 0);
    ~Message();

    long long id() const;
    bool our() const;

    const QString& text() const;
    const QDateTime& date() const;
    QString dateFormatted() const;
protected:
    long long   m_id;
    int         m_from_id;

    QString     m_text;
    QDateTime   m_date;
};

Q_DECLARE_METATYPE(Message*);
