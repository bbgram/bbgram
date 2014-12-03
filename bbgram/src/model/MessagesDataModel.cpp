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
        else
            return "message";
    }
    else
        return "header";
}
