#include "ApplicationUI.h"

#include <bb/cascades/NavigationPane>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/QmlDocument>

#include "ui/widgets/PhoneNumberInput.h"

#include "Telegraph.h"

#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

using namespace bb::cascades;
using namespace bb::system;

#include "model/Chat.h"
#include "model/Dialog.h"
#include "model/Message.h"
#include "model/User.h"

ApplicationUI::ApplicationUI(bb::cascades::Application* app) :
        QObject(app)
{
    qmlRegisterType<PhoneNumberInput>("bbgram.control.lib", 0, 1, "PhoneNumberInput");
    qmlRegisterType<Chat>("bbgram.types.lib", 0, 1, "Chat");
    qmlRegisterType<Dialog>("bbgram.types.lib", 0, 1, "Dialog");
    qmlRegisterType<Message>("bbgram.types.lib", 0, 1, "Message");
    qmlRegisterType<User>("bbgram.types.lib", 0, 1, "User");

    m_storage = new Storage(this);
    m_telegraph = new Telegraph();
    m_telegraph->start();

    QObject::connect(Telegraph::instance(), SIGNAL(mainDCAuthorized()), this, SLOT(onMainAuthorized()));
    QObject::connect(Telegraph::instance(), SIGNAL(allDCAuthorized()), this, SLOT(onAllAuthorized()));

    if (gTLS->DC_working && tgl_signed_dc(gTLS, gTLS->DC_working))
        showMainScreen();
    else
        showIntroScreen();
}

ApplicationUI::~ApplicationUI()
{
    m_telegraph->stop();
}

void ApplicationUI::onMainAuthorized()
{
    SystemToast *toast = new SystemToast(this);

    toast->setBody("Main DC Authorized");
    toast->setPosition(SystemUiPosition::MiddleCenter);
    toast->show();
}

void ApplicationUI::onAllAuthorized()
{
    SystemToast *toast = new SystemToast(this);

    toast->setBody("All DC Authorized");
    toast->setPosition(SystemUiPosition::MiddleCenter);
    toast->show();
}

#include "ui/IntroScreen.h"
#include "ui/MainScreen.h"

void ApplicationUI::showIntroScreen()
{
    IntroScreen* intro = new IntroScreen(this);
    intro->setContextProperty("_app", this);

    Application::instance()->setScene(intro->rootObject());
}

void ApplicationUI::showMainScreen()
{
    AbstractPane* prevPane = Application::instance()->scene();
    if (prevPane)
    {
        prevPane->setParent(NULL);
        prevPane->deleteLater();
    }

    MainScreen* main = new MainScreen(this);
    main->setContextProperty("_app", this);

    Application::instance()->setScene(main->rootObject());
}
