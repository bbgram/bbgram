#include "HeadlessApplication.hpp"
#include <bb/network/PushErrorCode>
#include "UDSWrapper.h"

//http://pushapi.eval.blackberry.com
HeadlessApplication::HeadlessApplication(bb::Application *app) :
    QObject(app), m_invokeManager(new InvokeManager())
{
    m_invokeManager->setParent(this);

    QObject::connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    m_pushService = new bb::network::PushService(QString(), "com.bigboss.bomogramHeadless", this);

    QObject::connect(m_pushService, SIGNAL(createSessionCompleted(const bb::network::PushStatus&)),
                        this, SLOT(onCreateSessionCompleted(const bb::network::PushStatus&)));

    QObject::connect(m_pushService, SIGNAL(createChannelCompleted(const bb::network::PushStatus&, const QString)),
                            this, SLOT(onCreateChannelCompleted(const bb::network::PushStatus&, const QString)));

    //m_pushService->createSession();
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
        qDebug() << request.data();
    }

}

void HeadlessApplication::onCreateSessionCompleted(const bb::network::PushStatus& status)
{
    qDebug() << status.errorDescription() << " ! "<< status.code() << endl;
    //if (status.code() == PushErrorCode::NoError)
    {
        //m_pushService->registerToLaunch();
        m_pushService->createChannel(QUrl());
    }
}

void HeadlessApplication::onCreateChannelCompleted(const bb::network::PushStatus& status, const QString token)
{
    qDebug() << status.errorDescription() << " ! "<< status.code() << "TOKEN " << token << endl;
}
