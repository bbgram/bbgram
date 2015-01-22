#include "Peer.h"
#include "MessagesDataModel.h"
#include "../Storage.h"
#include "../utils/Colorizer.h"

using namespace bb::cascades;

Peer::Peer(int type, int id)
    : m_type(type), m_id(id), m_muteUntil(0), m_loadingHistory(false), m_photoId(0), m_unreadCount(0)
{
    m_messages = new MessagesDataModel(this);
    m_messages->setSortingKeys(QStringList() << "date" << "dateTime");
    m_messages->setSortedAscending(false);
    connect(m_messages, SIGNAL(itemAdded(QVariantList)), this, SIGNAL(messagesChanged()));
    connect(m_messages, SIGNAL(itemRemoved(QVariantList)), this, SIGNAL(messagesChanged()));


    m_photoFilename = _getDefaultPlaceholder();
}

Peer::~Peer()
{
}

int Peer::type() const
{
    return m_type;
}

int Peer::id() const
{
    return m_id;
}

void Peer::deserialize(QByteArray& data)
{
    QDataStream stream(&data, QIODevice::ReadOnly);

    QVariantMap map;
    stream >> map;

    load(map);
}

QByteArray Peer::serialize() const
{
    QVariantMap map;
    save(map);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << map;
    return data;
}

bb::cascades::DataModel* Peer::messages() const
{
    return m_messages;
}

Message* Peer::lastMessage() const
{
    if (m_messages->size() > 0)
    {
        QVariant data = m_messages->data(QVariantList() << 0 << 0);
        return (Message*)data.value<QObject*>();
    }
    return 0;
}

QDateTime Peer::lastMessageDate() const
{
    Message* last = lastMessage();
    return last ? last->dateTime() : QDateTime();
}

void Peer::addMessage(Message* message)
{
    if (!message->our() && message->unread())
    {
        m_unreadCount++;
        qDebug() << m_unreadCount;
        emit unreadCountChanged();
    }

    m_messages->insert(message);
}

void Peer::deleteMessage(Message* message)
{
    m_messages->remove(message);
}

int Peer::unreadCount()
{
    return m_unreadCount;
}

void Peer::markRead()
{
    for (QVariantList indexPath = m_messages->first(); !indexPath.isEmpty(); indexPath = m_messages->after(indexPath))
    {
        Message* message = (Message*)m_messages->data(indexPath).value<QObject*>();
        if (!message->our() && message->unread())
        {
            message->markAsRead();
            Storage::instance()->saveMessage(message);
        }
    }

    m_unreadCount = 0;
    emit unreadCountChanged();
}

void Peer::save(QVariantMap& map) const
{
    QList<QVariant> list;
    for (int i = 0; i < m_lapseMarkers.size(); i++)
        list.append(m_lapseMarkers[i]);
    map.insert("lapseMarkers", list);

    QVariantMap notifySettings;
    notifySettings["muteUntil"] = m_muteUntil;
    notifySettings["sound"] = m_sound;
    notifySettings["showPreviews"] = m_showPreviews;
    notifySettings["eventsMasks"] = m_eventsMasks;
    map.insert("notifySettings", notifySettings);

    map.insert("photo", m_photoFilename);
    map.insert("photoId", m_photoId);
}

void Peer::load(const QVariantMap& map)
{
    QVariantMap::const_iterator it;
    it = map.find("lapseMarkers");

    if (it != map.end())
    {
        m_lapseMarkers.clear();
        QList<QVariant> list = it.value().toList();
        for (int i = 0; i < list.size(); i++)
            m_lapseMarkers.append(list[i].toInt());
    }

    it = map.find("notifySettings");
    if (it != map.end())
    {
        QVariantMap notifySettings = it.value().toMap();
        it = notifySettings.find("muteUntil");
        if (it != notifySettings.end())
            m_muteUntil = it.value().toUInt();

        it = notifySettings.find("sound");
        if (it != notifySettings.end())
            m_sound = it.value().toString();

        it = notifySettings.find("showPreviews");
        if (it != notifySettings.end())
            m_showPreviews = it.value().toInt();

        it = notifySettings.find("eventsMasks");
        if (it != notifySettings.end())
            m_eventsMasks = it.value().toInt();
    }

    it = map.find("photo");
    if (it != map.end())
        setPhoto(it.value().toString());
    it = map.find("photoId");
    if (it != map.end())
        setPhotoId(it.value().toLongLong());
}

void Peer::loadAdditionalHistory()
{
    if (!m_loadingHistory && m_lapseMarkers.length() > 0)
        Storage::instance()->loadAdditionalHistory(this);
}

QUrl Peer::photo() const
{
    return QUrl("file://" + QDir::currentPath() + "/" + m_photoFilename);
}

const QString& Peer::photoFileName()
{
    return m_photoFilename;
}

void Peer::setPhoto(const QString &filename)
{
    if (m_photoFilename.compare(filename) == 0)
        return;

    if (filename == "")
    {
        setPhotoId(0);
        m_photoFilename = _getDefaultPlaceholder();
    }
    else
    {
        if (QFile::exists(filename))
            m_photoFilename = filename;
        else
        {
            setPhotoId(0);
            m_photoFilename = _getDefaultPlaceholder();
            Storage::instance()->AsyncPhotoLoad(this);
        }
    }

    emit photoChanged();
}

void Peer::setPhotoId(long long photoId)
{
    m_photoId = photoId;
}

long long Peer::getPhotoId() const
{
    return m_photoId;
}

bool Peer::muted() const
{
    if (m_muteUntil == 0)
        return false;
    else
        return QDateTime::currentDateTime() < QDateTime::fromTime_t(m_muteUntil);
}

void Peer::updateNotifySettings(uint mute_until, char* sound, int show_previews, int events_masks)
{
    m_muteUntil = mute_until;
    m_sound = QString::fromUtf8(sound);
    m_showPreviews = show_previews;
    m_eventsMasks = events_masks;
    emit notifySettingsChanged();
}

void Peer::mute(bool value)
{
    if (value)
    {
        QDateTime date = QDateTime::currentDateTime().addYears(1);
        m_muteUntil = date.toTime_t();
    }
    else
        m_muteUntil = 0;

    tgl_notify_peer_t notify_peer;
    notify_peer.type = tgl_notify_peer;
    notify_peer.peer.type = m_type;
    notify_peer.peer.id = m_id;

    QByteArray sound = m_sound.toLocal8Bit();
    tgl_do_update_notify_settings (gTLS, &notify_peer, (int)m_muteUntil, sound.data(), m_showPreviews, m_eventsMasks, 0, 0);

    Storage::instance()->markPeerDirty(this);
    emit notifySettingsChanged();
}

QString Peer::_getDefaultPlaceholder()
{
    switch(m_type)
    {
        case TGL_PEER_USER:
            return Colorizer::userPlaceholder(m_id);
        case TGL_PEER_CHAT:
            return Colorizer::groupPlaceholder(m_id);
    }

    return QString();
}
