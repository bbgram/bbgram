#include "ApplicationUI.h"

#include <bb/cascades/NavigationPane>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/QmlDocument>
#include <bb/data/JsonDataAccess>
#include <bb/system/CardDoneMessage>

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
#include "model/Wallpaper.h"

#include "ui/IntroScreen.h"
#include "ui/MainScreen.h"
#include "utils/VirtualKeyboardService.h"


static IntroScreen* introScreen = NULL;
static MainScreen* mainScreen = NULL;

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
    qmlRegisterType<Wallpaper>("bbgram.types.lib", 0, 1, "Wallpaper");
    qmlRegisterType<QTimer>("bbgram.types.lib", 0, 1, "QTimer");

    qmlRegisterType<VirtualKeyboardService>("bbgram.bps.lib", 0, 1, "VirtualKeyboardService");

    m_isCard = false;

    m_invokeManager = new InvokeManager(this);
    QObject::connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoke(const bb::system::InvokeRequest&)));

    m_telegraph = new Telegraph();
    m_telegraph->start();

    m_storage = new Storage(this);

    if (m_invokeManager->startupMode() == ApplicationStartupMode::InvokeCard)
        m_isCard = true;

    if (gTLS->DC_working && tgl_signed_dc(gTLS, gTLS->DC_working))
        showMainScreen(m_isCard);
    else
        showIntroScreen();

    if (mainScreen)
        QObject::connect(m_invokeManager, SIGNAL(cardPooled(const bb::system::CardDoneMessage &)), mainScreen, SLOT(onCardPooled(const bb::system::CardDoneMessage &)));
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

void ApplicationUI::onInvoke(const bb::system::InvokeRequest& invoke)
{
    bb::data::JsonDataAccess jda;
    QVariantMap objectMap = (jda.loadFromBuffer(invoke.data())).toMap();
    QVariantMap attributesMap = objectMap["attributes"].toMap();

    if(invoke.action().compare("bb.action.VIEW") == 0)
    {
        QString contactId = attributesMap["sourceId"].toString();

        long long peerId = contactId.toLongLong();

        int id = (int)peerId;
        int type = (peerId >> 32);

        Peer* peer = Storage::instance()->getPeer(type, id);

        if (peer)
        {
            mainScreen->openCardChat(peer);
        }
    }
}

void ApplicationUI::showIntroScreen()
{
    introScreen = new IntroScreen(this);
    introScreen->setContextProperty("_app", this);

    Application::instance()->setScene(introScreen->rootObject());
}

void ApplicationUI::showMainScreen(bool card)
{
    AbstractPane* prevPane = Application::instance()->scene();
    if (prevPane)
    {
        prevPane->setParent(NULL);
        prevPane->deleteLater();
    }

    delete introScreen;

    mainScreen = new MainScreen(this, card);
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

void ApplicationUI::sendCardDoneMessage()
{
    if (m_isCard)
    {
        // Assemble response message
        CardDoneMessage message;
        message.setData(tr(""));
        message.setDataType("text/plain");
        message.setReason(tr("Success!"));

        // Send message
        m_invokeManager->sendCardDone(message);
    }
}
