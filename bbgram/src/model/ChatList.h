#pragma once

#include <bb/cascades/GroupDataModel>
#include <bb/cascades/QListDataModel>

#include "User.h"
#include "GroupChat.h"

class ChatList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::GroupDataModel *model READ model CONSTANT);
    Q_PROPERTY(int filter READ filter NOTIFY filterChanged)
    //Q_PROPERTY(int telegramContactsCount READ telegramContactsCount NOTIFY contentUpdated)
public:
    ChatList(bb::cascades::QListDataModel<Peer*>* dialogs);
    ~ChatList();

    bb::cascades::GroupDataModel* model() const;

    int filter() const;
    Q_INVOKABLE void setFilter(int filter, const QString& text = "");
private slots:
    void itemAdded(const QVariantList& index);
    void itemRemoved(const QVariantList& index);
    void updateContent();
signals:
    void filterChanged();
    void contentUpdated();
protected:
    bb::cascades::GroupDataModel*           m_model;
    int                                     m_filter;
    QString                                 m_searchText;
    bb::cascades::QListDataModel<Peer*>*    m_dialogs;
};

