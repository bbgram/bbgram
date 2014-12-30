#pragma once
#include <bb/cascades/QListDataModel>
#include "../model/Peer.h"

class PeerDataModel : public bb::cascades::QListDataModel<Peer*>
{
    Q_OBJECT
public:
    void removeAt(int i)
    {
        Peer* item = bb::cascades::QListDataModel<Peer*>::value(i);
        bb::cascades::QListDataModel<Peer*>::removeAt(i);
        emit itemRemoved(item);
    }

signals:
    void itemRemoved(Peer* item);
};
