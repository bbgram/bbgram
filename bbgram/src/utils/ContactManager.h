#pragma once

#include <QObject>

class ContactManager: public QObject
{
    Q_OBJECT
public:
    ContactManager();
    ~ContactManager();

    Q_INVOKABLE
    void addContact(const QString& firstName, const QString& lastName, const QString& phone);

signals:
     void onContactAdded(bool error, QString message);

private:
    static void contactAddHandler(struct tgl_state *TLS, void *callback_extra, int success, int size, struct tgl_user *users[]);

    static ContactManager* m_instance;
};

//Q_DECLARE_METATYPE(ContactManager*);


