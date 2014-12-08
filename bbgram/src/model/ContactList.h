#pragma once

#include <bb/cascades/GroupDataModel>
#include <bb/cascades/QListDataModel>

#include "User.h"

class ContactList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::GroupDataModel *model READ model CONSTANT);
    Q_PROPERTY(int filter READ filter NOTIFY filterChanged)
    Q_PROPERTY(int telegramContactsCount READ telegramContactsCount NOTIFY contentUpdated)
public:
    ContactList(bb::cascades::QListDataModel<User*>* telegramContacts);
    ~ContactList();

    bb::cascades::GroupDataModel* model() const;

    void updatePhonebook();

    int filter() const;
    Q_INVOKABLE void setFilter(int filter, const QString& text = "");

    int telegramContactsCount() const;
private slots:
    void updateContent();
signals:
    void filterChanged();
    void contentUpdated();
protected:
    bb::cascades::GroupDataModel*           m_model;
    int                                     m_filter;
    QString                                 m_searchText;
    bb::cascades::QListDataModel<User*>*    m_telegramContacts;
    QList<QVariantMap>                      m_phoneBook;
};

