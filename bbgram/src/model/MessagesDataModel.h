#pragma once

#include <bb/cascades/GroupDataModel>

class MessagesDataModel: public bb::cascades::GroupDataModel
{
public:
    MessagesDataModel(QObject* parent);
    ~MessagesDataModel();

    QString itemType(const QVariantList &indexPath);
};
