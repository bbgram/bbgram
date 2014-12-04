#include "GroupChat.h"
#include "../utils/Colorizer.h"
#include "../Storage.h"

using namespace bb::cascades;

GroupChat::GroupChat(int id)
    : Chat(TGL_PEER_CHAT, id), m_adminId(0)
{
    setPhoto("");

    m_members = new GroupDataModel();
    m_members->setParent(this);
    m_members->setSortingKeys(QStringList() << "firstName");
    m_members->setGrouping(ItemGrouping::None);
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

int GroupChat::getAdmin() const
{
    return m_adminId;
}

void GroupChat::setAdmin(int adminId)
{
    m_adminId = adminId;

    emit adminChanged();
}

void GroupChat::addMember(User* newMember, int inviterId)
{
    m_members->insert(newMember);
    m_invites.insert(newMember->id(), inviterId);
}

void GroupChat::deleteMember(User* member)
{
    m_members->remove(member);
    m_invites.remove(member->id());

    emit membersChanged();
}

void GroupChat::setMembers(const tgl_chat_user* members, int count)
{
    m_members->clear();
    m_invites.clear();

    for (int i = 0; i < count; i++)
    {
        User* member = (User*)Storage::instance()->getPeer(TGL_PEER_USER, members[i].user_id);
        m_members->insert(member);
        m_invites.insert(members[i].user_id, members[i].inviter_id);
    }

    emit membersChanged();
}

bb::cascades::DataModel* GroupChat::members() const
{
    return m_members;
}

bool GroupChat::canDeleteUser(User* self, User* target) const
{
    if (self->id() == target->id() || target->id() == m_adminId)
        return false;

    if (self->id() == m_adminId)
        return true;

    int selfId = self->id();
    int targetId = target->id();

    QMap<int, int>::const_iterator it = m_invites.find(targetId);
    if (it != m_invites.end())
        if (it.value() == selfId)
            return true;

    return false;
}
