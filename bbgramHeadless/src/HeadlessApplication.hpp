#pragma once

#include <bb/system/InvokeManager>
#include <bb/network/PushService>
#include <bb/network/PushStatus>
#include <bb/Application>
#include <QObject>

using namespace bb::network;
using namespace bb::system;

class HeadlessApplication : public QObject
{
    Q_OBJECT
public:
    HeadlessApplication(bb::Application *app);

private slots:
    void onInvoked(const bb::system::InvokeRequest& request);
    void onCreateSessionCompleted(const bb::network::PushStatus& status);
    void onCreateChannelCompleted(const bb::network::PushStatus& status, const QString token);
private:
    InvokeManager* m_invokeManager;
    PushService *m_pushService;
};
