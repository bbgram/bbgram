#pragma once

#include "Peer.h"
#include "User.h"

#include <bb/cascades/DataModel>
#include <bb/cascades/QListDataModel>

class GroupChat : public Peer
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::DataModel *members READ members NOTIFY membersChanged);
public:
    GroupChat(int id = 0);
    ~GroupChat();

    void load(const QVariantMap& map);
    void save(QVariantMap& map) const;

    QString title() const;
    void setTitle(const QString& title);

    QString status() const;

    int getAdmin() const;
    void setAdmin(int adminId);

    void addMember(User* newMember, int inviterId);
    void deleteMember(User* member);
    void setMembers(const tgl_chat_user* members, int count);

    bb::cascades::DataModel* members() const;

    Q_INVOKABLE bool canDeleteUser(User* self, User* target) const;
signals:
    void adminChanged();
    void membersChanged();
protected:
    QString                         m_title;
    int                             m_adminId;
    bb::cascades::GroupDataModel*   m_members;
    //user inviter
    QMap<int, int>                  m_invites;
};

Q_DECLARE_METATYPE(GroupChat*);
