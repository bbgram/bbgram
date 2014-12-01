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

    initialize();
}

MainScreen::~MainScreen()
{
}

Q_INVOKABLE void MainScreen::updateContact(User* user, const QString& firstName, const QString& lastName)
{
    if (user->id() == gTLS->our_id)
        tgl_do_set_profile_name(gTLS, firstName.toUtf8().data(), lastName.toUtf8().data(), NULL, NULL);
}

Q_INVOKABLE void MainScreen::sendMessage(int peerType, int peerId, const QString& message)
{
    tgl_peer_id_t peer;
    peer.type = peerType;
    peer.id = peerId;
    QByteArray bytes = message.toUtf8();
    tgl_do_send_message(gTLS, peer, (const char*)bytes.data(), bytes.length(), 0, 0);
}

Q_INVOKABLE void MainScreen::openFAQ()
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
