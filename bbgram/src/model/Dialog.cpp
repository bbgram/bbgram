#include "Dialog.h"

Dialog::Dialog(User* user)
    : Chat(TGL_PEER_USER, user->id()), m_user(user)
{
    connect(m_user, SIGNAL(photoChanged()), this, SIGNAL(photoChanged()));

    m_title = m_user->firstName() + " " + m_user->lastName();
    emit titleChanged();

    /*tgl_message* last = m_user->source()->last;
    m_status = last ? QString::fromUtf8(last->message) : "";
    emit statusChanged();*/
}

Dialog::~Dialog()
{
}

User* Dialog::user() const
{
    return m_user;
}

QString Dialog::title() const
{
    return m_title;
}

QString Dialog::status() const
{
    return m_status;
}

QVariant Dialog::photo() const
{
    return m_user->photo();
}
