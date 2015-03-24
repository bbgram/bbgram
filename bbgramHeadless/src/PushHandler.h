#pragma once
#include <QObject>
#include <QMap>

class Handler : public QObject
{
    Q_OBJECT
public:
    Handler(QObject* parent)
        :QObject(parent)
    {}

    virtual bool handle(const QVariantMap& pushData) {return false;}
};

class PushHandler : public QObject
{
    Q_OBJECT
public:
    PushHandler();

    bool handle(const QVariantMap& pushRoot);

protected:
    QMap<QString, Handler*> m_handlers;
};
