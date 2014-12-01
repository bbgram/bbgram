#include "ContactList.h"

using namespace bb::cascades;

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

    updateContent();
}

ContactList::~ContactList()
{
}

GroupDataModel* ContactList::model() const
{
    return m_model;
}

int ContactList::filter() const
{
    return m_filter;
}

void ContactList::setFilter(int filter)
{
    m_filter = filter;
    emit filterChanged();
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
        for (int i = 0; i < 5; i++)
        {
            QStringList list;
            list << QString::fromUtf8("Василий") << QString::fromUtf8("Артём") << QString::fromUtf8("Константин") << QString::fromUtf8("Руслан") << QString::fromUtf8("Сигизмунд");
            QVariantMap entry;
            QString name = list.value(rand() % list.size());
            entry["sortingKey"] = name;
            entry["firstName"] = name;
            entry["lastName"] = QString::fromUtf8("Неизвесткин");
            entry["online"] = true;
            m_model->insert(entry);
        }
    }
}
