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

void GroupChat::update(tgl_chat *chat)
{
    m_title = QString::fromUtf8(chat->title);
    emit titleChanged();
}

QString GroupChat::title() const
{
    return m_title;
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
