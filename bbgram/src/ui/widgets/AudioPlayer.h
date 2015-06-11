#pragma once
#include <bb/cascades/CustomControl>
#include <bb/cascades/Container>

#include "../../model/Message.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <opusfile.h>

const int AUDIO_BUFFERS_COUNT = 8;

class AudioPlayer: public bb::cascades::CustomControl
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage)
    Q_PROPERTY(bool isDonwloaded READ isDonwloaded NOTIFY donwloadedChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY playingChanged)
public:
    AudioPlayer();
    virtual ~AudioPlayer();

    Message* message() const;
    void setMessage(Message *message);

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    Q_INVOKABLE bool isDonwloaded();
    Q_INVOKABLE bool isPlaying();
signals:
    void donwloadedChanged();
    void playingChanged();
protected slots:
    void updateBuffers();
protected:
    Message* m_message;
    bb::cascades::Container* m_root;
    QString m_soundFile;
    OggOpusFile* m_opusFile;
    QTimer m_updateBuffersTimer;
    ALuint m_source;
    ALuint m_buffers[AUDIO_BUFFERS_COUNT];
    bool m_isDownloaded;
    bool m_isPlaying;

    void setDonwloaded(bool value);
    void setPlaying(bool value);
    int _fillBuffer(ALuint buffer);
    static void loadAudioCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename);

    static ALCdevice* m_alDevice;
    static ALCcontext* m_alContext;
};

Q_DECLARE_METATYPE(AudioPlayer*);

