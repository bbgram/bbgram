#pragma once

#include <QObject>

#include "Message.h"

class Chat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant photo READ photo NOTIFY photoChanged)
    Q_PROPERTY(Message* lastMessage READ lastMessage NOTIFY messagesChanged)
public:
    Chat();
    ~Chat();

    virtual QString title() const;
    virtual QString status() const;
    virtual QVariant photo() const;
    Message* lastMessage() const;

    void addMessage(Message* message);
signals:
    void titleChanged();
    void statusChanged();
    void photoChanged();
    void messagesChanged();
protected:
    QList<Message*> m_messages;
};

Q_DECLARE_METATYPE(Chat*);
Q_DECLARE_METATYPE(QList<Message*>);
