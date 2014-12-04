#include "MainScreen.h"
#include "../Storage.h"

#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/PpsObject>

using namespace bb::cascades;
using namespace bb::system;

MainScreen* MainScreen::m_instance = NULL;

MainScreen::MainScreen(ApplicationUI* app)
    : Screen("asset:///ui/pages/Main.qml")
        , m_app(app)
{
    Q_ASSERT(m_instance == NULL);
    m_instance = this;

    m_contacts = new ContactList(Storage::instance()->contacts());
    setContextProperty("_contacts", m_contacts);
    setContextProperty("_chats", Storage::instance()->dialogs());

    m_contactManager = new ContactManager();
    setContextProperty("_contactManager", m_contactManager);

    initialize();
}

MainScreen::~MainScreen()
{
    delete m_contactManager;
    m_instance = NULL;
}

void MainScreen::sendMessage(Chat* chat, const QString& message)
{
    tgl_peer_id_t peer;
    peer.type = chat->type();
    peer.id = chat->id();
    QByteArray bytes = message.toUtf8();
    tgl_do_send_message(gTLS, peer, (const char*)bytes.data(), bytes.length(), 0, 0);
}

void MainScreen::deleteMessage(long long id)
{
    Storage::instance()->deleteMessage(id);
}

void MainScreen::markRead(Chat* chat)
{
    tgl_peer_id_t peer;
    peer.type = chat->type();
    peer.id = chat->id();
    tgl_do_mark_read(gTLS, peer, 0, 0);
}

void MainScreen::createGroup(QVariantList users, const QString& title)
{
    tgl_peer_id_t* peers = new tgl_peer_id_t[users.size()];

    int idx = 0;
    foreach (QVariant variant, users)
    {
        User* user = (User*)variant.value<QObject*>();
        tgl_peer_id_t peer = {TGL_PEER_USER, user->id()};
        peers[idx] = peer;
        idx++;
    }

    tgl_do_create_group_chat_ex(gTLS, users.size(), peers, title.toUtf8().data(), MainScreen::_createGroupCallback, peers);
}

void MainScreen::setGroupName(GroupChat* group, const QString& title)
{
    tgl_do_rename_chat(gTLS, {group->type(), group->id()}, title.toUtf8().data(), NULL, NULL);
}

void MainScreen::addUserToGroup(GroupChat* group, User* user)
{
    tgl_do_add_user_to_chat(gTLS, {group->type(), group->id()}, {user->type(), user->id()}, 0, MainScreen::_addMemberCallback, user);
}

void MainScreen::deleteUserFromGroup(GroupChat* group, User* user)
{
    tgl_do_del_user_from_chat(gTLS, {group->type(), group->id()}, {user->type(), user->id()}, MainScreen::_deleteMemberCallback, user);
}

void MainScreen::deleteChat(Chat* chat)
{
    qDebug() << "deleting " << chat->title();
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

void MainScreen::sendInviteText(const QString& recipient, const QString& text)
{
    InvokeRequest request;
    request.setTarget("sys.pim.text_messaging.composer");
    request.setAction("bb.action.COMPOSE");
    request.setMimeType("application/text_messaging");
    QVariantMap map;
    map.insert("to", QVariantList() << recipient);
    map.insert("body", text);
    map.insert("send", false);
    QByteArray requestData = bb::PpsObject::encode(map, NULL);
    request.setData(requestData);
    InvokeManager mgr;
    mgr.invoke(request);
}

void MainScreen::dialANumber(const QString& number)
{
    InvokeRequest request;
    request.setAction("bb.action.DIAL");
    request.setMimeType("application/vnd.blackberry.phone.startcall");
    QVariantMap map;
    map.insert("number", number);
    QByteArray requestData = bb::PpsObject::encode(map, NULL);
    request.setData(requestData);
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

    User* currentUser = (User*)Storage::instance()->getPeer(TGL_PEER_USER, gTLS->our_id);
    setContextProperty("_currentUser", currentUser);
}

void MainScreen::_createGroupCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M)
{
    delete[] (tgl_peer_id_t*)callback_extra;

    GroupChat* groupChat = NULL;

    if (success)
    {
        groupChat = (GroupChat*)Storage::instance()->getPeer(TGL_PEER_CHAT, M->to_id.id);
        groupChat->setAdmin(gTLS->our_id);
    }

    emit m_instance->groupCreated(groupChat);
}

void MainScreen::_addMemberCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M)
{
    if (!success)
        return;

    GroupChat* groupChat = (GroupChat*)Storage::instance()->getPeer(TGL_PEER_CHAT, M->to_id.id);
    groupChat->addMember((User*)callback_extra, TLS->our_id);
}

void MainScreen::_deleteMemberCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_message *M)
{
    if (!success)
        return;

    GroupChat* groupChat = (GroupChat*)Storage::instance()->getPeer(TGL_PEER_CHAT, M->to_id.id);
    groupChat->deleteMember((User*)callback_extra);
}
