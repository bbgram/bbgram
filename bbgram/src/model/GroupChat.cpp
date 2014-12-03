#include "GroupChat.h"

using namespace bb::cascades;

GroupChat::GroupChat(int id)
    : Chat(TGL_PEER_CHAT, id)
{
    QString path = QString("app/native/assets/images/placeholders/group_placeholder_green.png");
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray bytes = file.readAll();
    file.close();
    m_photo = Image(bytes);
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
        path = QString("app/native/assets/images/placeholders/group_placeholder_green.png");

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray bytes = file.readAll();
    file.close();
    m_photo = Image(bytes);

    emit photoChanged();
}
