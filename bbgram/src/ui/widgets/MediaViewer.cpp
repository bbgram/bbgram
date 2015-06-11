#include "MediaViewer.h"

using namespace bb::cascades;

MediaViewer::MediaViewer()
    : m_message(0), m_aspectRatio(0.f)
{
    m_imageView = new ImageView();
    m_imageView->setScalingMethod(ScalingMethod::AspectFit);
    setImplicitLayoutAnimationsEnabled(false);
    m_imageView->setImplicitLayoutAnimationsEnabled(false);
    m_imageView->setLoadEffect(ImageViewLoadEffect::None);
    m_imageView->setHorizontalAlignment(HorizontalAlignment::Fill);
    m_imageView->setVerticalAlignment(VerticalAlignment::Fill);
    setRoot(m_imageView);
}

MediaViewer::~MediaViewer()
{
}

Message* MediaViewer::message() const
{
    return m_message;
}

struct LoadPhotoExtra
{
    MediaViewer*    viewer;
    int             sizeIdx;
};

void MediaViewer::loadPhotoCallback(struct tgl_state *TLS, void *callback_extra, int success, const char *filename)
{
    LoadPhotoExtra* extra = (LoadPhotoExtra*)callback_extra;
    if (success)
    {
        QString filename_ = QString::fromUtf8(filename);
        /*QVariantList sizes = extra->viewer->m_message->media()["sizes"].toList();
        sizes[extra->sizeIdx].toMap().insert("filename", filename_);*/
        QUrl url = QUrl::fromLocalFile(filename_);
        extra->viewer->m_imageView->setImageSource(url);
    }
    delete extra;
}

void MediaViewer::setMessage(Message *message)
{
    m_message = message;
    if (m_message->mediaType() == tgl_message_media_photo)
    {
        const QVariantMap& media = message->media();

        QVariantList sizes = media["sizes"].toList();

        int max = -1;
        int maxi = -1;
        for (int i = 0; i < sizes.size(); i++)
        {
            QVariantMap size = sizes[i].toMap();
            int w = size["width"].toInt();
            int h = size["height"].toInt();

              if (w + h > max)
              {
                max = w + h;
                maxi = i;
              }
        }
        QVariantMap sz = sizes[maxi].toMap();

        m_height = sz["height"].toInt();
        m_width = sz["width"].toInt();
        m_aspectRatio = (float)m_height / (float)m_width;

        setPreferredWidth(m_width);
        setPreferredHeight(m_height);

        connect(this, SIGNAL(maxWidthChanged(float)), this, SLOT(updateImageView(float)));

        /*if (sz.find("filename") != sz.end())
        {
            QString filename = sz["filename"].toString();
            QUrl url = QUrl::fromLocalFile(filename);
            m_imageView->setImageSource(url);
        }
        else*/
        {
            tgl_photo_size photoSize;
            photoSize.w = m_width;
            photoSize.w = m_height;
            photoSize.size = sz["size"].toInt();
            photoSize.loc.volume = sz["volume"].toLongLong();
            photoSize.loc.dc = sz["dc"].toInt();
            photoSize.loc.local_id = sz["local_id"].toInt();
            photoSize.loc.secret = sz["secret"].toLongLong();

            LoadPhotoExtra* extra = new LoadPhotoExtra();
            extra->viewer = this;
            extra->sizeIdx = maxi;
            tgl_do_load_photo_size(gTLS, &photoSize, loadPhotoCallback, extra);
        }
    }
    /*@else if (m_message->mediaType() == tgl_message_media_photo_encr)
    {
        const QVariantMap& media = message->media();
        m_height = media["height"].toInt();
        m_width = media["width"].toInt();

        m_encKey = media["key"].toByteArray();
        m_encIv = media["iv"].toByteArray();
        m_encCaption = media["caption"].toByteArray();
        m_encMimeType = media["mime_type"].toByteArray();

        m_aspectRatio = (float)m_height / (float)m_width;

        setPreferredWidth(m_width);
        setPreferredHeight(m_height);

        connect(this, SIGNAL(maxWidthChanged(float)), this, SLOT(updateImageView(float)));

        if (media.find("filename") != media.end())
        {
            QString filename = media["filename"].toString();
            QUrl url = QUrl::fromLocalFile(filename);
            m_imageView->setImageSource(url);
        }
        else
        {
            tgl_encr_document encrPhoto;
            memset(&encrPhoto, 0, sizeof(tgl_encr_document));

            encrPhoto.w = m_width;
            encrPhoto.h = m_height;
            encrPhoto.id = media["id"].toLongLong();
            encrPhoto.size = media["size"].toInt();
            encrPhoto.dc_id = media["dc_id"].toInt();
            encrPhoto.flags = media["flags"].toInt();
            encrPhoto.access_hash = media["access_hash"].toLongLong();
            encrPhoto.key_fingerprint = media["key_fingerprint"].toInt();
            encrPhoto.key = (unsigned char*)m_encKey.data();
            encrPhoto.iv = (unsigned char*)m_encIv.data();
            encrPhoto.mime_type = m_encMimeType.data();
            encrPhoto.caption = m_encCaption.data();
            encrPhoto.duration =  media["duration"].toInt();

            tgl_do_load_encr_document(gTLS, &encrPhoto, loadPhotoCallback, this);
        }
    }*/
}

void MediaViewer::updateImageView(float maxWidth){
    float w = std::min((float)m_width, maxWidth);
    setPreferredWidth(w);
    setPreferredHeight(w * m_aspectRatio);
    setMaxWidth(w);
    setMaxHeight(w * m_aspectRatio);
}
