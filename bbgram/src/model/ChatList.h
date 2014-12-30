#pragma once

#include <bb/cascades/GroupDataModel>
#include <bb/cascades/QListDataModel>

#include "User.h"
#include "GroupChat.h"

#include "../utils/PeerDataModel.h"

class ChatList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::GroupDataModel *model READ model CONSTANT);
    Q_PROPERTY(int filter READ filter NOTIFY filterChanged)
    //Q_PROPERTY(int telegramContactsCount READ telegramContactsCount NOTIFY contentUpdated)
public:
    ChatList(PeerDataModel* dialogs);
    ~ChatList();

    bb::cascades::GroupDataModel* model() const;

    int filter() const;
    Q_INVOKABLE void setFilter(int filter, const QString& text = "");
private slots:
    void itemAdded(const QVariantList& index);
    void itemRemoved(Peer* peer);
    void updateContent();
signals:
    void filterChanged();
    void contentUpdated();
protected:
    bb::cascades::GroupDataModel*           m_model;
    int                                     m_filter;
    QString                                 m_searchText;
    PeerDataModel*    m_dialogs;
};

