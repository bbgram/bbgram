#pragma once

#include <QObject>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/Image>

#include <tgl.h>

#include "Message.h"

class Peer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int type READ type CONSTANT)
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl photo READ photo NOTIFY photoChanged)
    Q_PROPERTY(bb::cascades::DataModel* messages READ messages CONSTANT)
    Q_PROPERTY(Message* lastMessage READ lastMessage NOTIFY messagesChanged)
    Q_PROPERTY(QDateTime lastMessageDate READ lastMessageDate NOTIFY messagesChanged)
    Q_PROPERTY(bool muted READ muted NOTIFY notifySettingsChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
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
    int unreadCount();
    void markRead();

    Q_INVOKABLE void loadAdditionalHistory();

    QUrl photo() const;
    const QString& photoFileName();
    void setPhoto(const QString & filename);
    void setPhotoId(long long photoId);
    long long getPhotoId() const;

    bool muted() const;
    void updateNotifySettings(uint mute_until, char* sound, int show_previews, int events_masks);
    Q_INVOKABLE void mute(bool value);
signals:
    void titleChanged();
    void statusChanged();
    void photoChanged();
    void messagesChanged();
    void notifySettingsChanged();
    void unreadCountChanged();
protected:
    QString _getDefaultPlaceholder();
    int             m_type;
    int             m_id;

    uint            m_muteUntil;
    QString         m_sound;
    int             m_showPreviews;
    int             m_eventsMasks;
    int             m_unreadCount;

    friend class Storage;
    bb::cascades::GroupDataModel* m_messages;
    QList<int>    m_lapseMarkers;

    bool        m_loadingHistory;
    QString                 m_photoFilename;
    long long               m_photoId;
};

Q_DECLARE_METATYPE(Peer*);
