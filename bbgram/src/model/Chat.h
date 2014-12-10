#pragma once

#include <QObject>
#include <bb/cascades/GroupDataModel>

#include "Message.h"

class Chat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int type READ type CONSTANT)
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant photo READ photo NOTIFY photoChanged)
    Q_PROPERTY(bb::cascades::DataModel* messages READ messages CONSTANT)
    Q_PROPERTY(Message* lastMessage READ lastMessage NOTIFY messagesChanged)
    Q_PROPERTY(QDateTime lastMessageDate READ lastMessageDate NOTIFY messagesChanged)
public:
    Chat(int type = 0, int id = 0);
    ~Chat();

    int type() const;
    int id() const;

    void deserialize(QByteArray& data);
    QByteArray serialize() const;

    virtual void save(QVariantMap& map) const;
    virtual void load(const QVariantMap& map);

    bb::cascades::DataModel* messages() const;

    virtual QString title() const;
    virtual QString status() const;
    virtual QVariant photo() const;
    Message* lastMessage() const;
    QDateTime lastMessageDate() const;

    void addMessage(Message* message);
    void deleteMessage(Message* message);

    Q_INVOKABLE void loadAdditionalHistory();
signals:
    void titleChanged();
    void statusChanged();
    void photoChanged();
    void messagesChanged();
protected:
    int             m_type;
    int             m_id;

    friend class Storage;
    bb::cascades::GroupDataModel* m_messages;
    QList<int>    m_lapseMarkers;

    bool        m_loadingHistory;
};

Q_DECLARE_METATYPE(Chat*);
