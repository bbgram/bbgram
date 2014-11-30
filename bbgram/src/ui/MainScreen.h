#pragma once

#include "Screen.h"
#include "User.h"

#include <bb/cascades/TabbedPane>

class ApplicationUI;

class MainScreen : public Screen<bb::cascades::TabbedPane>
{
    Q_OBJECT
public:
    MainScreen(ApplicationUI* app);
    ~MainScreen();


    Q_INVOKABLE void updateContact(User* user, const QString& firstName, const QString& lastName);

    Q_INVOKABLE void openFAQ();
protected slots:
    void initialize();
protected:
    ApplicationUI*  m_app;
};
