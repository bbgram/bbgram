#pragma once

#include <QObject>
#include <bb/cascades/GroupDataModel>

#include <tgl.h>

#include "Message.h"

class Peer : public QObject
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
    Q_PROPERTY(bool muted READ muted NOTIFY notifySettingsChanged)
public:
    Peer(int type = 0, int id = 0);
    ~Peer();

    int type() const;
    int id() const;

    void deserialize(QByteArray& data);
    QByteArray serialize() const;

    virtual void save(QVariantMap& map) const;
    virtual void load(const QVariantMap& map);

    bb::cascades::DataModel* messages() const;

    virtual QString title() const { return QString(); }
    virtual QString status() const { return QString(); }
    Message* lastMessage() const;
    QDateTime lastMessageDate() const;

    void addMessage(Message* message);
    void deleteMessage(Message* message);

    Q_INVOKABLE void loadAdditionalHistory();

    virtual QVariant photo() const { return QVariant(); }
    virtual void setPhoto(const QString &) {}
    virtual void setPhotoId(long long) {}
    virtual long long getPhotoId() const { return 0; }

    bool muted() const;
    void updateNotifySettings(uint mute_until, char* sound, int show_previews, int events_masks);
    Q_INVOKABLE void mute(bool value);
signals:
    void titleChanged();
    void statusChanged();
    void photoChanged();
    void messagesChanged();
    void notifySettingsChanged();
protected:
    int             m_type;
    int             m_id;

    uint            m_muteUntil;
    QString         m_sound;
    int             m_showPreviews;
    int             m_eventsMasks;

    friend class Storage;
    bb::cascades::GroupDataModel* m_messages;
    QList<int>    m_lapseMarkers;

    bool        m_loadingHistory;
};

Q_DECLARE_METATYPE(Peer*);
