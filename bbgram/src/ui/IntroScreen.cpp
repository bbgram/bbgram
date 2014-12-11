#include "IntroScreen.h"

#include "../ApplicationUI.h"
#include "../Telegraph.h"

#include <bb/cascades/Dialog>

using namespace bb::cascades;

IntroScreen::IntroScreen(ApplicationUI* app)
    : Screen("asset:///ui/pages/Intro.qml")
        , m_app(app)
{
    m_phone = "";
    m_codeHash = "";
    m_registered = false;
}

IntroScreen::~IntroScreen()
{
}



const QString& IntroScreen::phone() const
{
    return m_phone;
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
        _this->m_app->showMainScreen();
    }
    else
    {
        NavigationPane* navigationPane = _this->m_rootObject;
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
        NavigationPane* navigationPane = this->m_rootObject;
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
