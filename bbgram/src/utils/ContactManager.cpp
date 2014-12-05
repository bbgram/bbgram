#include "ContactManager.h"
#include "../Storage.h"

ContactManager* ContactManager::m_instance = NULL;

ContactManager::ContactManager()
{
    m_instance = this;
}

ContactManager::~ContactManager()
{
    m_instance = NULL;
}

void ContactManager::addContact(const QString& firstName, const QString& lastName, const QString& phone)
{
    if (!contactExist(phone))
        tgl_do_add_contact(gTLS, phone.toUtf8().data(), phone.toUtf8().size(), firstName.toUtf8().data(), firstName.toUtf8().size(), lastName.toUtf8().data(), lastName.toUtf8().size(), false, ContactManager::_contactAddHandler, NULL);
    else
        emit m_instance->contactAdded(true, "Contact already exists");
}

void ContactManager::renameContact(const QString& firstName, const QString& lastName, const QString& phone)
{
    if (contactExist(phone))
        tgl_do_add_contact(gTLS, phone.toUtf8().data(), phone.toUtf8().size(), firstName.toUtf8().data(), firstName.toUtf8().size(), lastName.toUtf8().data(), lastName.toUtf8().size(), false, ContactManager::_contactRenameHandler, NULL);
    else
        emit m_instance->contactRenamed(true, "Contact not exist");
}

void ContactManager::deleteContact(User* contact)
{
    tgl_peer_id_t peer;
    peer.type = contact->type();
    peer.id = contact->id();
    tgl_do_del_contact(gTLS, peer, ContactManager::_contactDeleteHandler, contact);
}

bool ContactManager::contactExist(const QString& phone)
{
    bb::cascades::QListDataModel<User*>* contacts = Storage::instance()->contacts();

    QString phone_filtred(phone);
    phone_filtred.replace("+","");
    phone_filtred.replace(" ","");

    bool found = false;
    for (int i = 0; i < contacts->size(); i++)
    {
        User* contact = contacts->value(i);
        if (contact->phone() == phone_filtred)
        {
            found = true;
            break;
        }
    }

    return found;
}

void ContactManager::_contactAddHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[])
{
    emit m_instance->contactAdded(size == 0, "Somthing wrong");
}

void ContactManager::_contactRenameHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[])
{
    emit m_instance->contactRenamed(size == 0, "Somthing wrong");
}

void ContactManager::_contactDeleteHandler(struct tgl_state *TLS, void *callback_extra, int success)
{
    if (success)
    {
        tgl_peer_id_t peer;
        peer.type = ((User*)callback_extra)->type();
        peer.id = ((User*)callback_extra)->id();
        tgl_do_get_user_info(gTLS, peer, false, NULL, NULL);
        //Storage::instance()->deleteContact((User*)callback_extra);
    }

    emit m_instance->contactDeleted(!success, "Somthing wrong");
}
