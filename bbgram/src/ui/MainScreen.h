#pragma once

#include "Screen.h"
#include "../model/Chat.h"
#include "../model/User.h"
#include "../utils/ContactManager.h"

#include <bb/cascades/TabbedPane>

class ApplicationUI;

class MainScreen : public Screen<bb::cascades::TabbedPane>
{
    Q_OBJECT
public:
    MainScreen(ApplicationUI* app);
    ~MainScreen();

    Q_INVOKABLE void sendMessage(Chat* chat, const QString& message);
    Q_INVOKABLE void markRead(Chat* chat);
    Q_INVOKABLE void openFAQ();
protected slots:
    void initialize();
protected:
    ApplicationUI*  m_app;
    ContactManager* m_contactManager;
};
