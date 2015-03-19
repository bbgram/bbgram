#include "UDSWrapper.h"
#include <bb/pim/account/AccountService>
#include <bb/pim/account/Account>
#include <bb/pim/account/Provider>
#include <bb/pim/account/Result>

#include "model/User.h"
#include "Storage.h"

const int OPEN_CAHT_CARD = 1;

using namespace bb::pim::account;

uds_context_t UDSWrapper::m_udsHandle = 0;
uds_account_key_t UDSWrapper::m_accountId = 0;

void UDSWrapper::initialize()
{
    int result = -1;

    result = uds_init(&m_udsHandle, false);
    if (result == UDS_SUCCESS)
    {
        const char* serviceURL = "bomogram";
        const char* libPath = "";
        const char* assetPath = "hubassets";

        QString tmpPath = QDir::current().absoluteFilePath("data");
        tmpPath = tmpPath.replace("/data", "/public/");
        tmpPath = tmpPath.append(assetPath);
        tmpPath = tmpPath.append("/");
        tmpPath = tmpPath.replace("/accounts/1000/appdata", "/apps");

        int status = 0;
        int serviceId = 0;

        result = uds_register_client(m_udsHandle, serviceURL, libPath, tmpPath.toUtf8().data());
        if (result == UDS_SUCCESS)
        {
            serviceId = uds_get_service_id(m_udsHandle);
            status = uds_get_service_status(m_udsHandle);

            if (status == UDS_REGISTRATION_NEW)
            {
                /* If this is a new registration, your app should add all of its data to the BlackBerry Hub */
            } else if (status == UDS_REGISTRATION_EXISTS)
            {
                /* If this is an existing registration, your app doesn't need to add all of its data again */
            }

            AccountService accountService;

            QList<Account> accounts = accountService.accounts();

            foreach(Account account, accounts)
            {
                if (account.isExternalData())
                {
                    account = accountService.account(account.id());
                    if (account.displayName() == "Bomogram" && account.externalSetupInvokeTarget() == "com.bigboss.bbgram")
                    {
                        m_accountId = account.id();
                        break;
                    }
                }
            }


            if (m_accountId == 0)
            {
                Provider provider = accountService.provider("external");
                Account account(provider);
                account.setDisplayName("Bomogram");
                account.setExternalData(true);
                account.setExternalSetupInvokeTarget("com.bigboss.bbgram");

                Result r = accountService.createAccount(provider.id(), account);

                if (r.isSuccess())
                {
                    m_accountId = account.id();
                }
            }

            if (m_accountId != 0)
            {
                uds_account_data_t *account_data = uds_account_data_create();
                uds_account_data_set_id(account_data, m_accountId);
                uds_account_data_set_name(account_data, "Bomogram");
                uds_account_data_set_icon(account_data, "hub_icon_v1.png");

                result = uds_account_added(m_udsHandle, account_data);
                if (result == UDS_SUCCESS)
                {
                    /*uds_account_action_data_t* account_action_data = uds_account_action_data_create();
                    uds_account_action_data_set_action(account_action_data, "bb.action.STATUS");
                    uds_account_action_data_set_target(account_action_data,"com.bigboss.bbgram");
                    uds_account_action_data_set_type(account_action_data,"");
                    uds_account_action_data_set_title(account_action_data,"Status");
                    uds_account_action_data_set_mime_type(account_action_data,"text/plain");
                    uds_account_action_data_set_placement(account_action_data,UDS_PLACEMENT_BAR);

                    result = uds_register_account_action(m_udsHandle, m_accountId, account_action_data);

                    uds_account_action_data_destroy(account_action_data);*/
                }
                else if (result == UDS_ERROR_FAILED)
                {
                    result = uds_account_updated(m_udsHandle, account_data);
                }
                uds_account_data_destroy(account_data);

                /*uds_item_action_data_t* action_data = uds_item_action_data_create();
               uds_item_action_data_set_action(action_data, "bb.action.COMPOSE");
               uds_item_action_data_set_target(action_data, "com.bigboss.bbgram.card");
               uds_item_action_data_set_title(action_data, "Open Chat Card");
               uds_item_action_data_set_mime_type(action_data,"hub/vnd.test.item");
               uds_item_action_data_set_context_mask(action_data, OPEN_CAHT_CARD);
               uds_item_action_data_set_type(action_data, "card.previewer");
               if (UDS_SUCCESS!= (result = uds_register_item_context_action(m_udsHandle, m_accountId , action_data)))
               {
                   printf("uds_register_item_context_action failed with error %d\n", result);
               }
               uds_item_action_data_destroy(action_data);*/
            }
        }
    }
}

void UDSWrapper::messageToHUB(Message* msg)
{
    uds_inbox_item_data_t* inboxItem = uds_inbox_item_data_create();
    uds_inbox_item_data_set_account_id(inboxItem, m_accountId);

    long long peerId = 0;
    if (msg->toType() == TGL_PEER_USER)
        peerId =  ((long long)msg->from()->type() << 32) | (unsigned int)msg->from()->id();
    else if (msg->toType() == TGL_PEER_CHAT)
        peerId =  ((long long)msg->toType() << 32) | (unsigned int)msg->toId();

    QString source = QString::number(peerId);

    QByteArray sourceBytes = source.toUtf8();
    uds_inbox_item_data_set_source_id(inboxItem, sourceBytes.data());

    QString name;
    if (msg->toType() == TGL_PEER_USER)
        name = msg->from()->title();
    else if (msg->toType() == TGL_PEER_CHAT)
    {
        GroupChat* group = (GroupChat*)Storage::instance()->getPeer(msg->toType(), msg->toId());
        name = msg->from()->title() + "@" + group->title();
    }

    QByteArray nameBytes = name.toUtf8();
    uds_inbox_item_data_set_name(inboxItem, nameBytes.data());
    QByteArray text = msg->text().toUtf8();
    uds_inbox_item_data_set_description(inboxItem, text.data());
    //uds_inbox_item_data_set_icon(inboxItem, "icon.png"); //msg->unread() ? "bar_voice2.png" : "bar_voice1.png");
    uds_inbox_item_data_set_mime_type(inboxItem, "hub/vnd.test.item");
    uds_inbox_item_data_set_unread_count(inboxItem, 0/*msg->unread() ? 1 : 0*/);
    uds_inbox_item_data_set_total_count(inboxItem, 1);
    //uds_inbox_item_data_set_category_id(inboxItem, 1);
    uds_inbox_item_data_set_timestamp(inboxItem, msg->dateTime().toTime_t());
    //uds_inbox_item_data_set_context_state(inboxItem, OPEN_CAHT_CARD);
    uds_item_removed(m_udsHandle, m_accountId, source.toUtf8().data());
    if (UDS_ERROR_FAILED == uds_item_added(m_udsHandle, inboxItem))
    {
        //bool result = UDS_SUCCESS == uds_item_updated(m_udsHandle, inboxItem);
    }

    uds_inbox_item_data_destroy(inboxItem);
}
