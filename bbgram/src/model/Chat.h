#pragma once

#include <QObject>
#include <bb/cascades/QListDataModel>

#include "Message.h"

class Chat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant photo READ photo NOTIFY photoChanged)
    Q_PROPERTY(bb::cascades::DataModel* messages READ messages CONSTANT)
    Q_PROPERTY(Message* lastMessage READ lastMessage NOTIFY messagesChanged)
public:
    Chat(int type = 0, int id = 0);
    ~Chat();

    int type() const;
    int id() const;

    bb::cascades::DataModel* messages() const;

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
    int             m_type;
    int             m_id;

    friend class Storage;
    bb::cascades::QListDataModel<Message*>* m_messages;
};

Q_DECLARE_METATYPE(Chat*);
Q_DECLARE_METATYPE(QList<Message*>);
