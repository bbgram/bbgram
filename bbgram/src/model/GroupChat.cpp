#include "GroupChat.h"
#include "../utils/Colorizer.h"

using namespace bb::cascades;

GroupChat::GroupChat(int id)
    : Chat(TGL_PEER_CHAT, id), m_admin(NULL)
{
    setPhoto("");
}

GroupChat::~GroupChat()
{
}

QString GroupChat::title() const
{
    return m_title;
}

void GroupChat::setTitle(const QString& title)
{
    m_title = title;
    emit titleChanged();
}

QString GroupChat::status() const
{
    Message* message = lastMessage();
    if (message)
        return "<p>" + message->text() + "</p>";
    return "";
}

QVariant GroupChat::photo() const
{
    return QVariant::fromValue(m_photo);
}

void GroupChat::setPhoto(const QString &filename)
{
    if (!m_photo.isNull() && m_photoFilename.compare(filename) == 0)
        return;
    m_photoFilename = filename;
    QString path;
    if (filename.length() != 0)
        path = filename;
    else
        path = Colorizer::groupPlaceholder(m_id);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray bytes = file.readAll();
    file.close();
    m_photo = Image(bytes);

    emit photoChanged();
}

User* GroupChat::admin() const
{
    return m_admin;
}

void GroupChat::setAdmin(User* admin)
{
    m_admin = admin;
    emit adminChanged();
}

void GroupChat::setMembers(const QList<User*>& members)
{
    m_members.clear();
    foreach(User* member, members)
        m_members.push_back(member);

    emit membersChanged();
}

const QList<User*>& GroupChat::getMembers() const
{
    return m_members;
}
