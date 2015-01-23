#include "ContactList.h"
#include "../ui/MainScreen.h"

#include <bb/pim/contacts/ContactService>
#include <bb/pim/contacts/Contact>
#include <bb/pim/contacts/ContactBuilder>
#include <bb/pim/contacts/ContactAttributeBuilder>

using namespace bb::cascades;
using namespace bb::pim::contacts;

ContactList::ContactList(PeerDataModel* telegramContacts)
    : m_filter(3), m_telegramContacts(telegramContacts)
{
    m_model = new GroupDataModel(this);
    m_model->setSortingKeys(QStringList() << "sortingKey" << "firstName");

    connect(m_telegramContacts, SIGNAL(itemAdded(QVariantList)), SLOT(itemAdded(QVariantList)));
    //connect(m_telegramContacts, SIGNAL(itemMoved(QVariantList, QVariantList)), SLOT(updateContent()));
    connect(m_telegramContacts, SIGNAL(itemRemoved(Peer*)), SLOT(itemRemoved(Peer*)));
    //connect(m_telegramContacts, SIGNAL(itemUpdated(QVariantList)), SLOT(updateContent()));
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

    ContactListFilters contactFilter;
    QSet<AttributeKind::Type> attrs;
    attrs.insert(AttributeKind::Name);
    attrs.insert(AttributeKind::Phone);
    contactFilter.setHasAttribute(attrs);
    QList<Contact> phonebook = service.contacts(contactFilter);

    m_phoneBook.clear();

    foreach (Contact contact, phonebook)
    {
        QList<ContactAttribute> phoneNumber = contact.phoneNumbers();
        if(phoneNumber.empty())
            continue;

        QVariantMap entry;
        entry["sortingKey"] = contact.firstName();
        entry["firstName"] = contact.firstName();
        entry["lastName"] = contact.lastName();
        entry["online"] = true;
        entry["phone"] = phoneNumber[0].value();

        qDebug() << contact.firstName() << " " << contact.lastName();
        m_phoneBook.push_back(entry);

        //MainScreen::instance()->addContact(contact.firstName(), contact.lastName(), phoneNumber[0].value());
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

void ContactList::itemAdded(const QVariantList& index)
{
    Peer* user = m_telegramContacts->data(index).value<Peer*>();
    m_model->insert(user);
}

void ContactList::itemRemoved(Peer* peer)
{
    m_model->remove(peer);
}

void ContactList::updateContent()
{
    m_model->clear();
    if (m_filter & 1)
    {
        for (int i = 0; i < m_telegramContacts->size(); i++)
        {
            User* user = (User*)m_telegramContacts->value(i);
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

    if ((m_filter & 4) && m_model->size() && m_searchText.size())
    {
        QVariantList it = m_model->last();

        while(!it.empty())
        {
            Peer* contact =  (Peer*)m_model->data(it).value<QObject*>();
            bool found = false;
            if (contact)
            {
                found = contact->title().indexOf(m_searchText) != -1;
            }
            else
            {
                QVariantMap map = m_model->data(it).value<QVariantMap>();

                QString str;
                str += map["firstName"].value<QString>();
                str += " ";
                str += map["lastName"].value<QString>();

                found = str.indexOf(m_searchText) != -1;
            }

            if (!found)
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
