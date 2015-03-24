#pragma once
#include <bb/pim/unified/unified_data_source.h>
#include <QString>

struct UDSWrapper
{
    static void initialize();

    static void messageToHub(const QString& source, const QString& name, const QString& text, bool is_unread = true);

    //static void messageToHUB(Message* msg);

    static uds_context_t m_udsHandle;
    static uds_account_key_t m_accountId;
};
