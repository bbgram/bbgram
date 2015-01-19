#include "HeadlessApplication.hpp"

HeadlessApplication::HeadlessApplication(bb::Application *app) :
    QObject(app), _invokeManager(new InvokeManager())
{
    _invokeManager->setParent(this);

    QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    qDebug() << "HeadlessApplication::HeadlessApplication" << endl;
}

void HeadlessApplication::onInvoked(const bb::system::InvokeRequest& request)
{
    // When the headless app is invoked, this function is called.
    // Add your app's logic here.
    qDebug() << "Hello Headless App" << endl;
}
