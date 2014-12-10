#include "User.h"
#include "../utils/Colorizer.h"

using namespace bb::cascades;

User::User(int id)
    : Chat(TGL_PEER_USER, id), m_online(false), m_typingStatus(tgl_typing_none), m_photoId(0)
{
    setPhoto("");

    connect(this, SIGNAL(titleChanged()), this, SIGNAL(nameChanged()));
    connect(this, SIGNAL(typingStatusChanged()), this, SIGNAL(statusChanged()));
    connect(this, SIGNAL(messagesChanged()), this, SIGNAL(statusChanged()));
}

User::~User()
{
}

void User::load(const QVariantMap& map)
{
    Chat::load(map);

    QVariantMap::const_iterator it;

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
    it = map.find("photoId");
    if (it != map.end())
        setPhotoId(it.value().toLongLong());
}

void User::save(QVariantMap& map) const
{
    Chat::save(map);

    map.insert("phone", m_phone);
    map.insert("firstName", m_firstName);
    map.insert("lastName", m_lastName);
    map.insert("photo", m_photoFilename);
    map.insert("photoId", m_photoId);
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

QString User::lastSeenFormatted() const
{
    QLocale locale(QLocale::English);
    QString str;
    if (m_lastSeen.date() == QDate::currentDate())
        str = "today";
    else
        str = locale.toString(m_lastSeen, "MMM dd");
    str += " at " + locale.toString(m_lastSeen, "hh:mm");
    return str;
}

QString User::status() const
{
    if (m_typingStatus != tgl_typing_none)
        return "<p style='color:#236EBB'>" + typingStatus() + "</p>";
    Message* message = lastMessage();
    if (message)
        return "<p>" + message->text() + "</p>";
    return "";
}

void User::setStatus(bool online, const QDateTime& lastSeen)
{
    m_online = online;
    m_lastSeen = lastSeen;
    emit statusChanged();
}

QString User::title() const
{
    return m_firstName + " " + m_lastName;
}

QString User::typingStatus() const
{
    switch (m_typingStatus)
    {
        case tgl_typing_typing:
            return "typing...";
        case tgl_typing_record_video:
            return "recording a video message...";
        case tgl_typing_upload_video:
            return "sending a video message...";
        case tgl_typing_record_audio:
            return "recording a voice message...";
        case tgl_typing_upload_audio:
            return "sending a voice message...";
        case tgl_typing_upload_photo:
            return "sending a photo...";
        case tgl_typing_upload_document:
            return "sending a document...";
        case tgl_typing_geo:
            return "sending ageo...";
        case tgl_typing_choose_contact:
            return "choosing a contact...";
        default:
            return "";
    }
}

void User::setTypingStatus(tgl_typing_status typingStatus)
{
    if (typingStatus == tgl_typing_cancel)
        m_typingStatus = tgl_typing_none;
    else
    {
        m_typingStatus = typingStatus;
        QTimer::singleShot(5000, this, SLOT(resetTypingStatus()));
    }

    emit typingStatusChanged();
}

void User::resetTypingStatus()
{
    m_typingStatus = tgl_typing_none;
    emit typingStatusChanged();
}

QVariant User::photo() const
{
    return QVariant::fromValue(m_photo);
}

void User::setPhotoId(long long photoId)
{
    m_photoId = photoId;
}

long long User::getPhotoId() const
{
    return m_photoId;
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
        path = Colorizer::userPlaceholder(m_id);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray bytes = file.readAll();
    file.close();
    m_photo = Image(bytes);

    emit photoChanged();
}

const QString& User::sortingKey() const
{
    static QString source = "!";
    return source;
}

QString User::color() const
{
    return Colorizer::color(m_id);
}
