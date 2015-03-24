#include "HeadlessApplication.hpp"
#include <bb/network/PushErrorCode>
#include <bb/network/PushPayload>
#include <bb/data/JsonDataAccess>
#include "UDSWrapper.h"

//http://pushapi.eval.blackberry.com
HeadlessApplication::HeadlessApplication(bb::Application *app) :
    QObject(app), m_invokeManager(new InvokeManager())
{
    m_invokeManager->setParent(this);

    QObject::connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    m_pushService = new bb::network::PushService(QString("5242-68a5060D78R34O443rr71i92cc3107aa556"), "com.bigboss.bomogramHeadless", this);

    QObject::connect(m_pushService, SIGNAL(createSessionCompleted(const bb::network::PushStatus&)),
                        this, SLOT(onCreateSessionCompleted(const bb::network::PushStatus&)));

    QObject::connect(m_pushService, SIGNAL(createChannelCompleted(const bb::network::PushStatus&, const QString)),
                            this, SLOT(onCreateChannelCompleted(const bb::network::PushStatus&, const QString)));

    m_pushService->createSession();
    //m_pushService->createChannel(QUrl());

    qDebug() << "HeadlessApplication::HeadlessApplication" << endl;
}

void HeadlessApplication::onInvoked(const bb::system::InvokeRequest& request)
{
    qDebug() << request.action() << endl;
    if(request.action().compare("bb.action.system.STARTED") == 0)
    {
        UDSWrapper::initialize();
    }
    else if(request.action().compare("bb.action.PUSH") == 0)
    {
        bb::network::PushPayload payload(request);
        qDebug() << "PushPayload " << QString(payload.data());

        bb::data::JsonDataAccess jda;
        QVariantMap rootMap = (jda.loadFromBuffer(payload.data())).toMap();
        QVariantMap dataMap = rootMap["data"].toMap();

    }

}

void HeadlessApplication::onCreateSessionCompleted(const bb::network::PushStatus& status)
{
    qDebug() << "HeadlessApplication::onCreateSessionCompleted";
    qDebug() << status.errorDescription() << " ! "<< status.code() << endl;
    if (status.code() == PushErrorCode::NoError)
    {
        //m_pushService->registerToLaunch();
        m_pushService->createChannel(QUrl("http://cp5242.pushapi.eval.blackberry.com"));
    }
}

void HeadlessApplication::onCreateChannelCompleted(const bb::network::PushStatus& status, const QString token)
{
    qDebug() << "HeadlessApplication::onCreateChannelCompleted";
    qDebug() << status.errorDescription() << " ! "<< status.code() << "TOKEN " << token << endl;
}
