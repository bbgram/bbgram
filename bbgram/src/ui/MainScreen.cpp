#include "MainScreen.h"
#include "../Storage.h"

#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>

using namespace bb::cascades;
using namespace bb::system;

MainScreen::MainScreen(ApplicationUI* app)
    : Screen("asset:///ui/pages/Main.qml")
        , m_app(app)
{
    setContextProperty("_contacts", Storage::instance()->contacts());
    setContextProperty("_chats", Storage::instance()->chats());

    m_contactManager = new ContactManager();
    setContextProperty("_contactManager", m_contactManager);

    initialize();
}

MainScreen::~MainScreen()
{
    delete m_contactManager;
}

void MainScreen::sendMessage(Chat* chat, const QString& message)
{
    tgl_peer_id_t peer;
    peer.type = chat->type();
    peer.id = chat->id();
    QByteArray bytes = message.toUtf8();
    tgl_do_send_message(gTLS, peer, (const char*)bytes.data(), bytes.length(), 0, 0);
}

void MainScreen::markRead(Chat* chat)
{
    tgl_peer_id_t peer;
    peer.type = chat->type();
    peer.id = chat->id();
    tgl_do_mark_read(gTLS, peer, 0, 0);
}

void MainScreen::openFAQ()
{
    InvokeRequest request;
    request.setTarget("sys.browser");
    request.setAction("bb.action.OPEN");
    request.setUri("http://telegram.org/faq");
    InvokeManager mgr;
    mgr.invoke(request);
}

void MainScreen::initialize()
{
    if (!gTLS->DC_working || !tgl_signed_dc(gTLS, gTLS->DC_working))
    {
        QTimer::singleShot(200, this, SLOT(initialize()));
        return;
    }

    tgl_do_get_difference(gTLS, 0, 0, 0);
    Storage::instance()->updateChats();
    Storage::instance()->updateContacts();
    Storage::instance()->updateUserInfo();

    User* currentUser = Storage::instance()->addUser(gTLS->our_id);
    setContextProperty("_currentUser", currentUser);
}
