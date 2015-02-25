#include "ApplicationUI.h"

#include <bb/cascades/NavigationPane>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/QmlDocument>

#include "ui/widgets/DocumentViewer.h"
#include "ui/widgets/MediaViewer.h"
#include "ui/widgets/PhoneNumberInput.h"

#include "Telegraph.h"

#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>

using namespace bb::cascades;
using namespace bb::system;

#include "model/Peer.h"
#include "model/GroupChat.h"
#include "model/Message.h"
#include "model/User.h"
#include "model/EncrChat.h"
#include "model/Document.h"
#include "model/Media.h"

ApplicationUI::ApplicationUI(bb::cascades::Application* app) :
        QObject(app)
{
    qmlRegisterType<PhoneNumberInput>("bbgram.control.lib", 0, 1, "PhoneNumberInput");
    qmlRegisterType<MediaViewer>("bbgram.control.lib", 0, 1, "MediaViewer");
    qmlRegisterType<DocumentViewer>("bbgram.control.lib", 0, 1, "DocumentViewer");
    qmlRegisterType<Peer>("bbgram.types.lib", 0, 1, "Peer");
    qmlRegisterType<GroupChat>("bbgram.types.lib", 0, 1, "GroupChat");
    qmlRegisterType<Message>("bbgram.types.lib", 0, 1, "Message");
    qmlRegisterType<User>("bbgram.types.lib", 0, 1, "User");
    qmlRegisterType<EncrChat>("bbgram.types.lib", 0, 1, "EncrChat");
    qmlRegisterType<Document>("bbgram.types.lib", 0, 1, "Document");
    qmlRegisterType<Media>("bbgram.types.lib", 0, 1, "Media");
    qmlRegisterType<QTimer>("bbgram.types.lib", 0, 1, "QTimer");

    m_telegraph = new Telegraph();
    m_telegraph->start();

    m_storage = new Storage(this);

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

static IntroScreen* introScreen = NULL;
static MainScreen* mainScreen = NULL;

void ApplicationUI::showIntroScreen()
{
    introScreen = new IntroScreen(this);
    introScreen->setContextProperty("_app", this);

    Application::instance()->setScene(introScreen->rootObject());
}

void ApplicationUI::showMainScreen()
{
    AbstractPane* prevPane = Application::instance()->scene();
    if (prevPane)
    {
        prevPane->setParent(NULL);
        prevPane->deleteLater();
    }

    delete introScreen;

    mainScreen = new MainScreen(this);
    mainScreen->setContextProperty("_app", this);

    Application::instance()->setScene(mainScreen->rootObject());
}

void ApplicationUI::logout()
{
    AbstractPane* prevPane = Application::instance()->scene();

    if (prevPane)
    {
        prevPane->setParent(NULL);
        prevPane->deleteLater();
    }

    delete mainScreen;

    m_storage->setParent(NULL);
    delete m_storage;

    m_telegraph->stop();
    delete m_telegraph;

    QFile::remove("data/storage.db");
    QFile::remove("data/auth.dat");
    QFile::remove("data/state.dat");

    m_telegraph = new Telegraph();
    m_telegraph->start();

    m_storage = new Storage(this);

    //delete

    QObject::connect(Telegraph::instance(), SIGNAL(mainDCAuthorized()), this, SLOT(onMainAuthorized()));
    QObject::connect(Telegraph::instance(), SIGNAL(allDCAuthorized()), this, SLOT(onAllAuthorized()));

    showIntroScreen();

}
