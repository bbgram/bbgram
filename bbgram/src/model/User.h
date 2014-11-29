#pragma once

#include <QObject>
#include <QMap>
#include <bb/cascades/Image>

class User : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString phone READ phone NOTIFY phoneChanged)
    Q_PROPERTY(QString firstName READ firstName NOTIFY nameChanged)
    Q_PROPERTY(QString lastName READ lastName NOTIFY nameChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(bool online READ online NOTIFY statusChanged)
    Q_PROPERTY(QDateTime lastSeen READ lastSeen NOTIFY statusChanged)
    Q_PROPERTY(QString lastSeenFormatted READ lastSeenFormatted NOTIFY statusChanged)
    Q_PROPERTY(QVariant photo READ photo NOTIFY photoChanged)
public:
    User(int id = 0);
    ~User();

    void deserialize(QByteArray& data);
    QByteArray serialize() const;

    int id() const;

    const QString& phone() const;
    void setPhone(const QString& phone);

    const QString& firstName() const;
    const QString& lastName() const;
    QString name() const;
    void setName(const QString& firstName, const QString& lastName);

    bool online() const;
    const QDateTime& lastSeen() const;
    QString lastSeenFormatted() const;
    void setStatus(bool online, const QDateTime& lastSeen);

    void setPhoto(const QString &filename);
    QVariant photo() const;
signals:
    void phoneChanged();
    void nameChanged();
    void statusChanged();
    void photoChanged();
protected:
    int         m_id;
    QString     m_phone;
    QString     m_firstName;
    QString     m_lastName;

    bool        m_online;
    QDateTime   m_lastSeen;

    QString                 m_photoFilename;
    bb::cascades::Image     m_photo;
};

Q_DECLARE_METATYPE(User*);
