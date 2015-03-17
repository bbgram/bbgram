#include "IntroScreen.h"

#include "../ApplicationUI.h"
#include "../Telegraph.h"

#include <bb/cascades/Dialog>

using namespace bb::cascades;

IntroScreen::IntroScreen(ApplicationUI* app)
    : m_app(app), m_authReady(false)
{
    m_qmlDocument = bb::cascades::QmlDocument::create("asset:///ui/pages/Intro.qml").parent(this);
    m_qmlDocument->setContextProperty("_owner", this);
    m_rootObject = m_qmlDocument->createRootObject<bb::cascades::NavigationPane>();

    m_phone = "";
    m_codeHash = "";
    m_registered = false;

    QObject::connect(&m_checkTimer, SIGNAL(timeout()), this, SLOT(checkConnection()));
    m_checkTimer.start();

    setContextProperty("_introScreen", this);
}

IntroScreen::~IntroScreen()
{
}

const QString& IntroScreen::phone() const
{
    return m_phone;
}

bool IntroScreen::authReady() const
{
    return m_authReady;
}

void IntroScreen::checkConnection()
{
    for (int i = 0; i < sizeof(gTLS->DC_list) / sizeof(gTLS->DC_list[0]); i++)
    {
        if (gTLS->DC_list[i] && gTLS->DC_list[i]->state != st_authorized)
        {
            return;
        }
    }
    m_checkTimer.stop();

    m_authReady = true;
    emit authReadyChanged();
}

void IntroScreen::_requestCodeCallback(struct tgl_state *TLS, void *callback_extra, int success, int registered, const char *hash)
{
    IntroScreen* _this = (IntroScreen*)callback_extra;
    Q_UNUSED(TLS);
    if (success)
    {
        _this->m_registered = registered;
        _this->m_codeHash = hash;
    }
}

void IntroScreen::requestCode(const QString& phone)
{
    m_phone = phone;
    emit phoneChanged();

    QByteArray user = phone.toLocal8Bit();
    tgl_do_send_code(gTLS, (const char*)user.data(), _requestCodeCallback, this);
}

void IntroScreen::requestPhoneCall()
{
    QByteArray _user = m_phone.toLocal8Bit();
    QByteArray _hash = m_codeHash.toLocal8Bit();
    tgl_do_phone_call(gTLS, _user.data(), _hash.data(), 0, 0);
}

void IntroScreen::_sendCodeCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *self)
{
    IntroScreen* _this = (IntroScreen*)callback_extra;
    if (success)
    {
        _this->m_app->showMainScreen(false);
    }
    else
    {
        NavigationPane* navigationPane = (NavigationPane*)_this->m_rootObject;
        navigationPane->setBackButtonsVisible(true);
        while (navigationPane->count() > 3)
            navigationPane->pop();

        emit _this->wrongCode();
    }
}

void IntroScreen::submitCode(const QString& code)
{
    QByteArray _user = m_phone.toLocal8Bit();
    QByteArray _hash = m_codeHash.toLocal8Bit();
    QByteArray _code = code.toLocal8Bit();
    m_code = code;
    if (m_registered)
        tgl_do_send_code_result(gTLS, _user.data(), _hash.data(), _code.data(), _sendCodeCallback, this);
    else if (m_firstName.size() && m_lastName.size())
    {
        tgl_do_send_code_result_auth(gTLS, _user.data(), _hash.data(), _code.data(), m_firstName.toUtf8().data(), m_lastName.toUtf8().data(), _sendCodeCallback, this);
    }
    else
    {
        NavigationPane* navigationPane = (NavigationPane*)this->m_rootObject;
        navigationPane->setBackButtonsVisible(true);

        QmlDocument* qml = QmlDocument::create("asset:///ui/pages/Registration.qml");
        Page* page = qml->createRootObject<Page>();
        qml->setContextProperty("_owner", this);
        navigationPane->push(page);
    }
}

void IntroScreen::registerUser(const QString& firstName, const QString& lastName, const QString& photo)
{
    QByteArray _user = m_phone.toLocal8Bit();
    QByteArray _hash = m_codeHash.toLocal8Bit();
    QByteArray _code = m_code.toLocal8Bit();
    QByteArray _firstName = firstName.toLocal8Bit();
    QByteArray _lastName = lastName.toLocal8Bit();
    m_firstName = firstName;
    m_lastName = lastName;

    tgl_do_send_code_result_auth(gTLS, _user.data(), _hash.data(), _code.data(), _firstName.data(), _lastName.data(), _sendCodeCallback, this);
}
