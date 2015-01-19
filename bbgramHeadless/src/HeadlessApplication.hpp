#pragma once

#include <bb/system/InvokeManager>
#include <bb/Application>
#include <QObject>

using namespace bb::system;

class HeadlessApplication : public QObject
{
    Q_OBJECT

public:
    HeadlessApplication(bb::Application *app);

private slots:
    void onInvoked(const bb::system::InvokeRequest& request);

private:
    InvokeManager* _invokeManager;
};
