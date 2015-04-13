#pragma once
#include <bb/cascades/CustomControl>
#include <bb/cascades/Container>

#include "../../model/Message.h"

class AudioPlayer: public bb::cascades::CustomControl
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage)
public:
    AudioPlayer();
    virtual ~AudioPlayer();

    Message* message() const;
    void setMessage(Message *message);
protected:
    Message* m_message;
    bb::cascades::Container* m_root;

    static void loadAudioCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename);
};
