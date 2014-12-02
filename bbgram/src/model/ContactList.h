#pragma once

#include <bb/cascades/GroupDataModel>
#include <bb/cascades/QListDataModel>

#include "User.h"

class ContactList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::GroupDataModel *model READ model CONSTANT);
    Q_PROPERTY(int filter READ filter WRITE setFilter NOTIFY filterChanged)
public:
    ContactList(bb::cascades::QListDataModel<User*>* telegramContacts);
    ~ContactList();

    bb::cascades::GroupDataModel* model() const;

    void updatePhonebook();

    int filter() const;
    void setFilter(int filter);
private slots:
    void updateContent();
signals:
    void filterChanged();
protected:
    bb::cascades::GroupDataModel*   m_model;
    int                             m_filter;
    bb::cascades::QListDataModel<User*>* m_telegramContacts;
    QList<QVariantMap> mPhoneBook;
};

