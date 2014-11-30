#include "Message.h"
#include <QLocale>

Message::Message(long long id, tgl_message* M)
    : m_id(id)
{
    m_text = QString::fromUtf8(M->message);
    m_date = QDateTime::fromTime_t(M->date);
}

Message::~Message()
{
}

long long Message::id() const
{
    return m_id;
}

const QDateTime& Message::date() const
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
    QDate currentDate = QDate::currentDate();
    if (date == currentDate)
        str = locale.toString(m_date, "hh:mm");
    else
    {
        if (date.daysTo(currentDate) < 7)
            str = locale.toString(m_date, "ddd");
        else
            str = locale.toString(m_date, "MMM dd");
    }
    return str;
}
