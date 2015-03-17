#pragma once

#include <QObject>
#include <bb/cascades/Application>
#include <bb/system/InvokeManager>

#include "Storage.h"
#include "Telegraph.h"

class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application* app);
    virtual ~ApplicationUI();

    Q_INVOKABLE void showIntroScreen();
    Q_INVOKABLE void showMainScreen(bool card);
    Q_INVOKABLE void logout();

    void sendCardDoneMessage();
public slots:
    void onMainAuthorized();
    void onAllAuthorized();
    void onInvoke(const bb::system::InvokeRequest& invoke);
protected:
    Telegraph*  m_telegraph;
    Storage*    m_storage;
    bb::system::InvokeManager* m_invokeManager;
    bool        m_isCard;
};
