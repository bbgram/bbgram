#pragma once

#include <QtSql>
#include <bb/cascades/QListDataModel>

#include "model/Peer.h"
#include "model/GroupChat.h"
#include "model/Message.h"
#include "model/User.h"
#include "model/EncrChat.h"
#include "model/BroadcastChat.h"

#include "utils/PeerDataModel.h"

class Storage: public QObject
{
    Q_OBJECT
public:
    Storage(QObject* parent);
    ~Storage();

    static Storage* instance();

    static void userUpdateHandler(struct tgl_state *TLS, struct tgl_user *U, unsigned flags);
    static void messageReceivedHandler(struct tgl_state *TLS, struct tgl_message *M);
    static void userStatusUpdateHandler(struct tgl_state *TLS, struct tgl_user *U);
    static void userTypingHandler(struct tgl_state *TLS, struct tgl_user *U, enum tgl_typing_status status);
    static void updateChatHandler(struct tgl_state *TLS, struct tgl_chat *C, unsigned flags);
    static void markedReadHandler(struct tgl_state *TLS, int num, struct tgl_message *list[]);
    static void messagesDeletedHandler(struct tgl_state *TLS, int num, int list[]);
    static void notifySettingsUpdateHandler(struct tgl_state *TLS, struct tgl_notify_peer_t *notify_peer, int mute_until, char* sound, int show_previews, int events_masks);
    static void encrChatUpdate(struct tgl_state *TLS, struct tgl_secret_chat *C, unsigned flags);

    PeerDataModel* contacts() const;
    PeerDataModel* dialogs() const;

    Message* getMessage(long long id);
    Peer* getPeer(int type, int id);
    void markPeerDirty(Peer* peer);

    void deleteMessage(long long id);
    void saveMessage(Message* message);
    void addContact(User* contact);
    void deleteContact(User* contact);

    BroadcastChat* createBroadcast(QVariantList users);
    void createEncrChar(Peer* peer);

    void deleteHistory(Peer* peer);
    void deleteChat(Peer* peer);

    void updateChats();
    void updateContacts();
    void updateUserInfo();

    void loadAdditionalHistory(Peer* peer);
    void searchMessage(Peer* peer, int from, int to, int limit, int offset, const char *s);

    void AsyncPhotoLoad(Peer* peer);

    bb::cascades::DataModel* getSharedMedia(Peer* peer);
    bb::cascades::DataModel* getSharedFiles(Peer* peer);
signals:
    void newMessageReceived(const Message* message);
protected slots:
    void saveUpdatesToDatabase();
protected:
    static Storage* m_instance;
    QSqlDatabase    m_db;

    PeerDataModel*    m_contacts;
    PeerDataModel*    m_dialogs;

    QMap<long long, Peer*>                  m_peers;
    QMap<long long, Message*>               m_messages;

    QList<Peer*>    m_updatedPeers;
    QList<Peer*>    m_photosToLoad;
    QTimer*         m_saveTimer;
    int             m_photosInQuoue;

    static void _getDialogsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, tgl_peer_id_t peers[], int last_msg_id[], int unread_count[]);
    static void _getHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[]);
    static void _deleteMessageCallback(struct tgl_state *TLS, void *callback_extra, int success);
    static void _deleteHistoryCallback(struct tgl_state *TLS, void *callback_extra, int success, int offset);
    static void _updateContactsCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *contacts[]);
    static void _updateGroupPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_chat *C);
    static void _updateContactPhoto(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *U);
    static void _searchMessageCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[]);
    static void _loadPhotoCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename);
    static void _newSecretChatCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_secret_chat *E);
    static void _searchFilesCallback(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_message *list[]);

    void _LoadNextPhoto();
    void _PhotoLoaded();
public:
    static void _broadcastSended(struct tgl_state *TLS, void *extra, int success, int num, struct tgl_message *ML[]);
};
