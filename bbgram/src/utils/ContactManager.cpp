#include "utils/ContactManager.h"

ContactManager* ContactManager::m_instance = NULL;

ContactManager::ContactManager()
{
    m_instance = this;
}

ContactManager::~ContactManager()
{
    m_instance = NULL;
}

Q_INVOKABLE
void ContactManager::addContact(const QString& firstName, const QString& lastName, const QString& phone)
{
    tgl_do_add_contact(gTLS, phone.toUtf8().data(), phone.toUtf8().size(), firstName.toUtf8().data(), firstName.toUtf8().size(), lastName.toUtf8().data(), lastName.toUtf8().size(), false, ContactManager::contactAddHandler, NULL);
}

void ContactManager::contactAddHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[])
{
    m_instance->emit onContactAdded(size == 0, "Somthing wrong");
}
