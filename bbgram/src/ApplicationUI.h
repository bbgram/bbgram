#pragma once

#include <QObject>
#include <bb/cascades/Application>

#include "Storage.h"
#include "Telegraph.h"

class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application* app);
    virtual ~ApplicationUI();

    Q_INVOKABLE void showIntroScreen();
    Q_INVOKABLE void showMainScreen();
public slots:
    void onMainAuthorized();
    void onAllAuthorized();
protected:
    Telegraph*  m_telegraph;
    Storage*    m_storage;
};
