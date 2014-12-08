#include "IntroScreen.h"

#include "../ApplicationUI.h"
#include "../Telegraph.h"

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


void IntroScreen::_sendCodeCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *self)
{
    IntroScreen* _this = (IntroScreen*)callback_extra;
    if (success)
    {
        _this->m_app->showMainScreen();
    }
    else
    {
        _this->m_codeHash = "";
        _this->m_registered = false;
        _this->m_phone = "";

        NavigationPane* navigationPane = _this->m_rootObject;
        navigationPane->setBackButtonsVisible(true);

        while (navigationPane->count() > 2)
            navigationPane->pop();
    }
}
/*
    QByteArray _user = phone.toLocal8Bit();
    QByteArray _hash = hash.toLocal8Bit();
    QByteArray _code = code.toLocal8Bit();
    QByteArray _firstName = firstName.toLocal8Bit();
    QByteArray _lastName = lastName.toLocal8Bit();
    tgl_do_send_code_result_auth(gTLS, _user.data(), _hash.data(), _code.data(), _firstName.data(), _lastName.data(), sendCodeResultCallback, callback);
*/

void IntroScreen::submitCode(const QString& code)
{
    QByteArray _user = m_phone.toLocal8Bit();
    QByteArray _hash = m_codeHash.toLocal8Bit();
    QByteArray _code = code.toLocal8Bit();
    tgl_do_send_code_result(gTLS, _user.data(), _hash.data(), _code.data(), _sendCodeCallback, this);
}
