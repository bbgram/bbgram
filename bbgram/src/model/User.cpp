#include "User.h"

using namespace bb::cascades;

User::User(int id)
    : m_id(id), m_online(false)
{
    setPhoto("");
}

User::~User()
{
}

void User::deserialize(QByteArray& data)
{
    QDataStream stream(&data, QIODevice::ReadOnly);

    QMap<QString, QVariant> map;
    stream >> map;

    QMap<QString, QVariant>::iterator it;

    it = map.find("phone");
    if (it != map.end())
        setPhone(it.value().toString());
    it = map.find("firstName");
    if (it != map.end())
        m_firstName = it.value().toString();
    it = map.find("lastName");
    if (it != map.end())
        m_lastName = it.value().toString();
    emit nameChanged();
    it = map.find("photo");
    if (it != map.end())
        setPhoto(it.value().toString());
}

QByteArray User::serialize() const
{
    QMap<QString, QVariant> map;

    map.insert("phone", m_phone);
    map.insert("firstName", m_firstName);
    map.insert("lastName", m_lastName);
    map.insert("photo", m_photoFilename);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << map;
    return data;
}

int User::id() const
{
    return m_id;
}

const QString& User::phone() const
{
    return m_phone;
}

void User::setPhone(const QString& phone)
{
    m_phone = phone;
    emit phoneChanged();
}

const QString& User::firstName() const
{
    return m_firstName;
}

const QString& User::lastName() const
{
    return m_lastName;
}

QString User::name() const
{
    QString name = m_firstName;
    if (m_lastName.length() > 0)
        name += " " + m_lastName;
    return name;
}

void User::setName(const QString& firstName, const QString& lastName)
{
    m_firstName = firstName;
    m_lastName = lastName;
    emit nameChanged();
}

bool User::online() const
{
    return m_online;
}

const QDateTime& User::lastSeen() const
{
    return m_lastSeen;
}

void User::setStatus(bool online, const QDateTime& lastSeen)
{
    m_online = online;
    m_lastSeen = lastSeen;
    emit statusChanged();
}

QVariant User::photo() const
{
    return QVariant::fromValue(m_photo);
}

void User::setPhoto(const QString &filename)
{
    if (!m_photo.isNull() && m_photoFilename.compare(filename) == 0)
        return;
    m_photoFilename = filename;
    QString path;
    if (filename.length() != 0)
        path = filename;
    else
        path = QString("app/native/assets/images/placeholders/user_placeholder_purple.png");

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray bytes = file.readAll();
    file.close();
    m_photo = Image(bytes);

    emit photoChanged();
}
