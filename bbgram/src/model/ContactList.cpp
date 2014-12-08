#include "ContactList.h"

#include <bb/pim/contacts/ContactService>
#include <bb/pim/contacts/Contact>
#include <bb/pim/contacts/ContactBuilder>
#include <bb/pim/contacts/ContactAttributeBuilder>

using namespace bb::cascades;
using namespace bb::pim::contacts;

ContactList::ContactList(QListDataModel<User*>* telegramContacts)
    : m_filter(3), m_telegramContacts(telegramContacts)
{
    m_model = new GroupDataModel(this);
    m_model->setSortingKeys(QStringList() << "sortingKey" << "firstName");

    connect(m_telegramContacts, SIGNAL(itemAdded(QVariantList)), SLOT(updateContent()));
    //connect(m_telegramContacts, SIGNAL(itemMoved(QVariantList, QVariantList)), SLOT(updateContent()));
    connect(m_telegramContacts, SIGNAL(itemRemoved(QVariantList)), SLOT(updateContent()));
    connect(m_telegramContacts, SIGNAL(itemUpdated(QVariantList)), SLOT(updateContent()));
    connect(this, SIGNAL(filterChanged()), SLOT(updateContent()));

    updatePhonebook();
    updateContent();
}

ContactList::~ContactList()
{
}

GroupDataModel* ContactList::model() const
{
    return m_model;
}

void ContactList::updatePhonebook()
{
    ContactService service;

    QSet<ContactId> excludedContacts;

    for (int i = 0; i < m_telegramContacts->size(); i++)
    {
        User* user = m_telegramContacts->value(i);
        ContactSearchFilters phoneSearch;
        phoneSearch.setHasAttribute(AttributeKind::Phone);
        phoneSearch.setSearchValue(QString("+") + user->phone());
        QList<Contact> users = service.searchContactsByPhoneNumber(phoneSearch);

        foreach (Contact contact, users)
        {
            excludedContacts.insert(contact.id());
        }

    }

    ContactListFilters contactFilter;
    QSet<AttributeKind::Type> attrs;
    attrs.insert(AttributeKind::Name);
    attrs.insert(AttributeKind::Phone);
    contactFilter.setHasAttribute(attrs);
    QList<Contact> phonebook = service.contacts(contactFilter);

    m_phoneBook.clear();

    foreach (Contact contact, phonebook)
    {
        bool found = false;
        foreach (ContactId id, excludedContacts)
            if (contact.id() == id)
            {
                found = true;
                break;
            }

        if (found)
            continue;

        QList<ContactAttribute> phoneNumber = contact.phoneNumbers();

        QVariantMap entry;
        entry["sortingKey"] = contact.firstName();
        entry["firstName"] = contact.firstName();
        entry["lastName"] = contact.lastName();
        entry["online"] = true;
        if(!phoneNumber.empty())
            entry["phone"] = phoneNumber[0].value();
        m_phoneBook.push_back(entry);
    }
}

int ContactList::filter() const
{
    return m_filter;
}

void ContactList::setFilter(int filter, const QString& text)
{
    m_filter = filter;
    m_searchText = text;
    emit filterChanged();
}

int ContactList::telegramContactsCount() const
{
    return m_telegramContacts->size();
}

void ContactList::updateContent()
{
    m_model->clear();
    if (m_filter & 1)
    {
        for (int i = 0; i < m_telegramContacts->size(); i++)
        {
            User* user = m_telegramContacts->value(i);
            m_model->insert(user);
        }
    }

    if (m_filter & 2)
    {
        QVariantMap contact;
        foreach (contact, m_phoneBook)
        {
            m_model->insert(contact);
        }
    }

    if ((m_filter & 4) && m_model->size())
    {
        QVariantList it = m_model->last();

        while(!it.empty())
        {
            Chat* contact =  (Chat*)m_model->data(it).value<QObject*>();
            if (contact->title().indexOf(m_filter) == -1)
            {
                QVariantList toRemove = it;
                it = m_model->before(it);
                m_model->removeAt(toRemove);
                continue;
            }
            else
                it = m_model->before(it);
        }

    }
    emit contentUpdated();
}
