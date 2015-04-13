#include "AudioPlayer.h"

#include <QtGlobal>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Container>

using namespace bb::cascades;

AudioPlayer::AudioPlayer()
: m_message(0)
{
    QmlDocument *qml = QmlDocument::create("asset:///ui/pages/chats/messages/AudioPlayer.qml").parent(this);
    m_root = qml->createRootObject<Container>();
    setRoot(m_root);
}

AudioPlayer::~AudioPlayer()
{
}

Message* AudioPlayer::message() const
{
    return m_message;
}

struct ExtraAudioData
{
    QByteArray      caption;
    QByteArray      mime;
    AudioPlayer*    player;
};

void AudioPlayer::setMessage(Message *message)
{
    m_message = message;
    const QVariantMap& media = message->media();

    if (message->mediaType() == tgl_message_media_document)
    {
        const QVariantMap& thumb = media["media"].toMap();

        tgl_document document;
        memset(&document, 0, sizeof(tgl_document));

        document.id = media["id"].toLongLong();
        document.access_hash = media["access_hash"].toLongLong();
        document.user_id = media["user_id"].toInt();
        document.date = media["date"].toInt();
        document.size = media["size"].toInt();
        document.dc_id = media["dc_id"].toInt();

        document.thumb.w = thumb["width"].toInt();
        document.thumb.h = thumb["height"].toInt();
        document.thumb.size = thumb["size"].toInt();
        document.thumb.loc.volume = thumb["volume"].toLongLong();
        document.thumb.loc.dc = thumb["dc"].toInt();
        document.thumb.loc.local_id = thumb["local_id"].toInt();
        document.thumb.loc.secret = thumb["secret"].toLongLong();

        ExtraAudioData* extraData = new ExtraAudioData;
        extraData->caption = media["caption"].toString().toUtf8();
        document.caption = extraData->caption.data();

        extraData->mime = media["mime_type"].toString().toUtf8();
        document.mime_type = extraData->mime.data();

        document.w = media["w"].toInt();
        document.h = media["h"].toInt();
        document.flags = media["flags"].toInt();
        document.duration = media["duration"].toInt();

        extraData->player = this;

        m_root->setProperty("duration", document.duration);

        tgl_do_load_document(gTLS, &document, AudioPlayer::loadAudioCallback, extraData);
    }
}

void AudioPlayer::loadAudioCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    ExtraAudioData* extraData = (ExtraAudioData*)callback_extra;

    if (success)
    {
        QString filename_ = "file://" + QDir::currentPath() + "/" + QString::fromUtf8(filename);
        extraData->player->m_root->setProperty("audioFileName", filename_);
    }
    else
    {
        int i = 1;
    }

    delete extraData;
}
