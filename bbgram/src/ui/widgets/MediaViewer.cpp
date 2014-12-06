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

void MediaViewer::loadPhotoCallback(struct tgl_state *TLS, void *callback_extra, int success, char *filename)
{
    if (!success)
        return;

    QString filename_ = QString::fromUtf8(filename);
    MediaViewer* instance = (MediaViewer*)callback_extra;
    instance->m_message->media().insert("filename", filename_);

    QUrl url = QUrl::fromLocalFile(filename_);
    instance->m_imageView->setImageSource(url);
}

void MediaViewer::setMessage(Message *message)
{
    m_message = message;
    if (m_message->mediaType() == tgl_message_media_photo)
    {
        const QVariantMap& media = message->media();
        /**/
        m_height = media["height"].toInt();
        m_width = media["width"].toInt();
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
            tgl_photo_size photoSize;
            photoSize.w = m_width;
            photoSize.w = m_height;
            photoSize.size = media["size"].toInt();
            photoSize.loc.volume =media["volume"].toLongLong();
            photoSize.loc.dc = media["dc"].toInt();
            photoSize.loc.local_id = media["local_id"].toInt();
            photoSize.loc.secret = media["secret"].toLongLong();

            tgl_photo photo;
            memset(&photo, 0, sizeof(tgl_photo));
            photo.sizes_num = 1;
            photo.sizes = &photoSize;

            tgl_do_load_photo(gTLS, &photo, loadPhotoCallback, this);
        }
    }
}

void MediaViewer::updateImageView(float maxWidth){
    float w = std::min((float)m_width, maxWidth);
    setPreferredWidth(w);
    setPreferredHeight(w * m_aspectRatio);
}
