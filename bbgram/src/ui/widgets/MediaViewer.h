#pragma once
#include <bb/cascades/CustomControl>
#include <bb/cascades/ImageView>
#include <bb/cascades/LayoutUpdateHandler>

#include "../../model/Message.h"

class MediaViewer: public bb::cascades::CustomControl
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage)
public:
    MediaViewer();
    virtual ~MediaViewer();

    Message* message() const;
    void setMessage(Message *message);
protected slots:
    void updateImageView(float maxWidth);
protected:
    Message* m_message;
    bb::cascades::ImageView*    m_imageView;
    float                       m_aspectRatio;
    int                         m_width;
    int                         m_height;

    QByteArray                  m_encKey;
    QByteArray                  m_encIv;
    QByteArray                  m_encCaption;
    QByteArray                  m_encMimeType;


    static void loadPhotoCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename);
};
