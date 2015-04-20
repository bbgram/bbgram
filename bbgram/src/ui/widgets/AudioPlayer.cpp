#include "AudioPlayer.h"

#include <QtGlobal>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Container>

using namespace bb::cascades;

ALCdevice* AudioPlayer::m_alDevice = NULL;
ALCcontext* AudioPlayer::m_alContext = NULL;

AudioPlayer::AudioPlayer()
: m_message(0), m_source(0)
{
    if (!m_alDevice)
    {
        const char * devicename = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
        m_alDevice = alcOpenDevice(devicename);
    }

    if (!m_alContext && m_alDevice)
    {
        m_alContext = alcCreateContext(m_alDevice, 0);
        alcMakeContextCurrent(m_alContext);

        alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
        alListenerf(AL_GAIN, 1.f);
    }

    connect(&m_updateBuffersTimer, SIGNAL(timeout()), this, SLOT(updateBuffers()));
    m_updateBuffersTimer.setSingleShot(false);

    m_isDownloaded = m_isPlaying = false;

    QmlDocument *qml = QmlDocument::create("asset:///ui/pages/chats/messages/AudioPlayer.qml").parent(this);
    m_root = qml->createRootObject<Container>();
    setRoot(m_root);

    /*const int SRATE = 44100;
    const int SSIZE = 1024;

    ALbyte buffer[22050];
    ALint sample;

    alGetError();
    ALCdevice *device = alcCaptureOpenDevice(NULL, SRATE, AL_FORMAT_MONO8, SSIZE);
    //if (alGetError() != AL_NO_ERROR) {
    //    return;
    //}
    alcCaptureStart(device);

    while (true) {
        alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);
        alcCaptureSamples(device, (ALCvoid *)buffer, sample);

        // ... do something with the buffer
    }

    alcCaptureStop(device);
    alcCaptureCloseDevice(device);*/
}

AudioPlayer::~AudioPlayer()
{
    stop();
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
        m_root->setProperty("player", QVariant::fromValue(this));
        m_root->setProperty("incoming", !message->our());

        tgl_do_load_document(gTLS, &document, AudioPlayer::loadAudioCallback, extraData);
    }
}

void AudioPlayer::play()
{
    if (!m_isDownloaded || m_isPlaying)
        return;

    setPlaying(true);

    alGenSources(1, &m_source);
    alSource3f(m_source, AL_POSITION, 0.f, 0.f, 0.f);
    alSourcef(m_source, AL_GAIN, 1.0f);

    int error = 0;
    m_opusFile = op_open_file(m_soundFile.toUtf8().data(), &error);
    if (error)
        qDebug() << "Cannot read opus file: " << m_soundFile;

    // Get the number of channels in the current link.
    int num_channels = op_channel_count(m_opusFile, -1);

    // Get the number of samples (per channel) in the current link.
    int pcm_size = op_pcm_total(m_opusFile, -1);

    alGenBuffers(AUDIO_BUFFERS_COUNT, m_buffers);

    for (int cur_buf = 0; cur_buf < AUDIO_BUFFERS_COUNT; ++cur_buf)
        _fillBuffer(m_buffers[cur_buf]);

    alSourceQueueBuffers(m_source, AUDIO_BUFFERS_COUNT, m_buffers);

    alSourcePlay(m_source);

    if (alGetError() != AL_NO_ERROR)
    {
        qDebug() << "Could not play source!";
    }
    m_updateBuffersTimer.start(10);
}

void AudioPlayer::stop()
{
    if (!m_isPlaying)
        return;

    m_updateBuffersTimer.stop();
    alSourceUnqueueBuffers(m_source, AUDIO_BUFFERS_COUNT, m_buffers);

    // We have to delete the source here, as OpenAL soft seems to need
    // the source gone before the buffers. Perhaps this is just timing.
    alDeleteSources(1, &m_source);
    alDeleteBuffers(AUDIO_BUFFERS_COUNT, m_buffers);

    // Close the opus file.
    op_free(m_opusFile);
    m_opusFile = NULL;

    setPlaying(false);
}

void AudioPlayer::updateBuffers()
{
    int num_processed_buffers = 0;
    ALuint currentbuffer;

    // How many buffers do we need to fill?
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &num_processed_buffers);

    ALenum source_state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &source_state);
    if (source_state != AL_PLAYING)
    {
        qDebug() << "Source not playing!";
        alSourcePlay(m_source);
    }

    // Unqueue a finished buffer, fill it with new data, and re-add it to the end of the queue.
    while (num_processed_buffers--)
    {
        alSourceUnqueueBuffers(m_source, 1 ,&currentbuffer);

        if (_fillBuffer(currentbuffer) <= 0)
        {
            stop();
        }

        alSourceQueueBuffers(m_source, 1, &currentbuffer);
    }
}

bool AudioPlayer::isDonwloaded()
{
    return m_isDownloaded;
}

void AudioPlayer::setDonwloaded(bool value)
{
    m_isDownloaded = value;
    emit donwloadedChanged();
}

bool AudioPlayer::isPlaying()
{
    return m_isPlaying;
}

void AudioPlayer::setPlaying(bool value)
{
    m_isPlaying = value;
    emit playingChanged();
}

int AudioPlayer::_fillBuffer(ALuint buffer)
{
    // Let's have a buffer that is two opus frames long (and two channels)
    const int buffer_size = 960 * 2 * 2;
    int16_t buf[buffer_size];

    int samples_read = 0;

    int num_channels = op_channel_count(m_opusFile, -1);

    //qDebug() << "Filling buffer: " << buffer;

    // We only support stereo and mono, set the openAL format based on channels.
    // opus always uses signed 16-bit integers, unless the _float functions are called.
    ALenum format;
    if (num_channels == 1)
    {
        format = AL_FORMAT_MONO16;
    }
    else if (num_channels == 2)
    {
        format = AL_FORMAT_STEREO16;
    }
    else
    {
        qDebug() << "File contained more channels than we support (" << num_channels << ")";
        return OP_EIMPL;
    }

    // Keep reading samples until we have them all.
    while (samples_read < buffer_size)
    {
        // op_read returns number of samples read (per channel), and accepts number of samples which fit in the buffer, not number of bytes.
        int ns = op_read(m_opusFile, buf + samples_read * num_channels, (buffer_size - samples_read * num_channels), 0);
        if (ns < 0)
        {
            qDebug() << "Couldn't decode at offset: " << m_soundFile;
            return ns;
        }
        if (ns == 0)
            break;
        samples_read += ns;
    }

    alBufferData(buffer, format, buf, samples_read * num_channels * 2, 48000);

    return samples_read;
}

void AudioPlayer::loadAudioCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    ExtraAudioData* extraData = (ExtraAudioData*)callback_extra;

    if (success)
    {
        extraData->player->m_soundFile = /*"file://" + */QDir::currentPath() + "/" + QString::fromUtf8(filename);
        extraData->player->setDonwloaded(true);
    }

    delete extraData;
}
