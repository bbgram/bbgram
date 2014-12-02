#include "Message.h"
#include <QLocale>

Message::Message(long long id, tgl_message* M)
    : m_id(id)
{
    if (M->service == 0 && M->message)
        m_text = QString::fromUtf8(M->message);
    m_date = QDateTime::fromTime_t(M->date);
    m_from_id = M->from_id.id;
}

Message::~Message()
{
}

long long Message::id() const
{
    return m_id;
}

bool Message::our() const
{
    return m_from_id == gTLS->our_id;
}

const QDate& Message::date() const
{
    return m_date.date();
}

const QDateTime& Message::dateTime() const
{
    return m_date;
}

const QString& Message::text() const
{
    return m_text;
}

QString Message::dateFormatted() const
{
    QLocale locale(QLocale::English);
    QString str;
    QDate date = m_date.date();
    QDateTime currentDate = QDateTime::currentDateTime();
    if (m_date.secsTo(currentDate) < 24*60*60)
        str = locale.toString(m_date, "hh:mm");
    else
    {
        if (m_date.daysTo(currentDate) < 7)
            str = locale.toString(m_date, "ddd");
        else
            str = locale.toString(m_date, "MMM dd");
    }
    return str;
}
