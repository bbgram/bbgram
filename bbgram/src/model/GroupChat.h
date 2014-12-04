#pragma once

#include "Chat.h"
#include "User.h"

#include <bb/cascades/DataModel>
#include <bb/cascades/QListDataModel>

class GroupChat : public Chat
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::DataModel *members READ members NOTIFY membersChanged);
public:
    GroupChat(int id = 0);
    ~GroupChat();

    QString title() const;
    void setTitle(const QString& title);

    QString status() const;
    QVariant photo() const;
    void setPhoto(const QString &filename);

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
    bb::cascades::Image             m_photo;
    QString                         m_photoFilename;
    QString                         m_title;
    int                             m_adminId;
    bb::cascades::GroupDataModel*   m_members;
    //user inviter
    QMap<int, int>                  m_invites;
};

Q_DECLARE_METATYPE(GroupChat*);
