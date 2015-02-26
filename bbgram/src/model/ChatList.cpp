#include "ChatList.h"
#include "../Storage.h"

using namespace bb::cascades;

ChatList::ChatList(PeerDataModel* dialogs)
    : m_filter(3), m_dialogs(dialogs)
{
    m_model = new GroupDataModel(this);
    m_model->setGrouping(ItemGrouping::None);
    m_model->setSortingKeys(QStringList() << "lastMessageDate");
    m_model->setSortedAscending(false);

    connect(m_dialogs, SIGNAL(itemAdded(QVariantList)), SLOT(itemAdded(QVariantList)));
    connect(m_dialogs, SIGNAL(itemRemoved(Peer*)), SLOT(itemRemoved(Peer*)));
    connect(this, SIGNAL(filterChanged()), SLOT(updateContent()));

    updateContent();
}

ChatList::~ChatList()
{
}

GroupDataModel* ChatList::model() const
{
    return m_model;
}

int ChatList::filter() const
{
    return m_filter;
}

void ChatList::setFilter(int filter, const QString& text)
{
    m_filter = filter;
    m_searchText = text;
    emit filterChanged();
}

void ChatList::itemAdded(const QVariantList& index)
{
    Peer* dialog = m_dialogs->data(index).value<Peer*>();
    m_model->insert(dialog);
}

void ChatList::itemRemoved(Peer* peer)
{
    QVariantList it = m_model->last();
    while(!it.empty())
    {
        Peer* contact =  (Peer*)m_model->data(it).value<QObject*>();
        if (peer == contact)
        {
            m_model->removeAt(it);
            break;
        }
        it = m_model->before(it);
    }
}

void ChatList::updateContent()
{
    m_model->clear();
    if (m_filter & 1)
    {
        for (int i = 0; i < m_dialogs->size(); i++)
        {
            //Chat* chat = m_dialogs->value(i);
            m_model->insert(m_dialogs->value(i));
        }
    }

    if ((m_filter & 2) && m_model->size() && m_searchText.size())
    {
        QVariantList it = m_model->last();

        while(!it.empty())
        {
            Peer* contact =  (Peer*)m_model->data(it).value<QObject*>();
            if (contact->title().indexOf(m_searchText) == -1)
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

    /*if ((m_filter & 4) && m_model->size() && m_searchText.size())
    {
        QVariantList it = m_model->last();

        while(!it.empty())
        {
            Chat* contact =  (Chat*)m_model->data(it).value<QObject*>();
            GroupDataModel* messages = (GroupDataModel*)contact->messages();

            bool found = false;
            for (QVariantList indexPath = messages->first(); !indexPath.isEmpty(); indexPath = messages->after(indexPath))
            {
                Message* message = (Message*)messages->data(indexPath).value<QObject*>();
                if (message->text().indexOf(m_searchText) != -1)
                {
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                QVariantList toRemove = it;
                it = m_model->before(it);
                m_model->removeAt(toRemove);


                Storage::instance()->searchMessage(contact, 0, contact->lastMessageDate().toTime_t(), 1, 0, m_searchText.toUtf8().data());
            }
            else
                it = m_model->before(it);
        }
    }*/
    emit contentUpdated();
}
