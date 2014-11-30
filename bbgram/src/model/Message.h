#pragma once

#include <QDateTime>

class Message: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text NOTIFY messageChanged)
    Q_PROPERTY(QDateTime date READ date NOTIFY messageChanged)
    Q_PROPERTY(QString dateFormatted READ dateFormatted NOTIFY messageChanged)
public:
    Message(long long id = 0, tgl_message* M = 0);
    ~Message();

    long long id() const;

    const QString& text() const;
    const QDateTime& date() const;
    QString dateFormatted() const;
signals:
    void messageChanged();
protected:
    long long  m_id;

    QString     m_text;
    QDateTime   m_date;
};

Q_DECLARE_METATYPE(Message*);
