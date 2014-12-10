#pragma once

#include "Screen.h"

#include <bb/cascades/NavigationPane>

class ApplicationUI;

class IntroScreen : public Screen<bb::cascades::NavigationPane>
{
    Q_OBJECT

    Q_PROPERTY(QString phone READ phone NOTIFY phoneChanged)
public:
    IntroScreen(ApplicationUI* app);
    ~IntroScreen();

    const QString& phone() const;

    Q_INVOKABLE void requestCode(const QString& phone);
    Q_INVOKABLE void requestPhoneCall();
    Q_INVOKABLE void submitCode(const QString& code);
signals:
    void phoneChanged();
protected:
    ApplicationUI*  m_app;

    QString         m_phone;
    bool            m_registered;
    QString         m_codeHash;

    static void _requestCodeCallback(struct tgl_state *TLS, void *callback_extra, int success, int registered, const char *hash);
    static void _sendCodeCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *self);
};
