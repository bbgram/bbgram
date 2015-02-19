#include "MessagesDataModel.h"
#include "Message.h"

using namespace bb::cascades;

MessagesDataModel::MessagesDataModel(QObject* parent)
    : GroupDataModel(parent)
{
}

MessagesDataModel::~MessagesDataModel()
{
}


QString MessagesDataModel::itemType(const QVariantList &indexPath)
{
    QVariant item = data(indexPath);
    Message* message = (Message*)item.value<QObject*>();
    if (message)
    {
        if (message->service())
            return "service_message";
        else if (message->mediaType() != tgl_message_media_none)
        {
            if (message->mediaType() == tgl_message_media_photo || message->mediaType() == tgl_message_media_photo_encr)
                return "photo";
        }
        return "message";
    }
    else
        return "header";
}
