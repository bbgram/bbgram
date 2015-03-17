#include "HeadlessApplication.hpp"
#include <bb/network/PushErrorCode>

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
    // When the headless app is invoked, this function is called.
    // Add your app's logic here.
    qDebug() << "Hello Headless App" << endl;
    qDebug() << request.action() << endl;
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
    qDebug() << status.errorDescription() << " ! "<< status.code() << endl;
}
