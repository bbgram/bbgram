#include <PushHandler.h>
#include <qdebug.h>
#include "UDSWrapper.h"

/*class MessageText : public Handler
{
public:
    MessageText(QObject* parent)
            :Handler(parent)
        {}

    bool handle(const QVariantMap& pushData)
    {
        QVariantList args = pushData["loc_args"].toList();
        QString authtor = args[0].toString();
        QString message = args[1].toString();

        //send to hub message authtor
        qDebug() << authtor << message;

        return true;
    }
};*/

#define BEGIN_PUSH_HANDLER(Name) \
    class Name : public Handler \
    { public:                      \
        Name(QObject* parent) : Handler(parent) {}

#define END_PUSH_HANDLER };


BEGIN_PUSH_HANDLER(MessageText)
bool handle(const QVariantMap& pushData)
{
    QVariantList args = pushData["loc_args"].toList();
    QString authtor = args[0].toString();
    QString message = args[1].toString();

    int chat_id = pushData["custom"].toMap()["chat_id"].toInt();

    //send to hub message authtor
    qDebug() << authtor << message;

    UDSWrapper::messageToHub(QString(chat_id), authtor, message, true);

    return true;
}
END_PUSH_HANDLER

PushHandler::PushHandler()
{
    m_handlers.insert("MESSAGE_TEXT", new MessageText(this));
}

bool PushHandler::handle(const QVariantMap& pushRoot)
{
    QVariantMap dataMap = pushRoot["data"].toMap();
    QString loc_key = dataMap["loc_key"].toString();

    if (loc_key.length() == 0)
        return false;

    qDebug() << "PushHandler::handle: " << loc_key;

    QMap<QString, Handler*>::iterator it = m_handlers.find(loc_key);
    if(it != m_handlers.end())
    {
        Handler* handler = it.value();
        return handler->handle(dataMap);
    }

    return false;
}
