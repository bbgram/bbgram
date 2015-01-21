#pragma once

#include "Screen.h"
#include <bb/cascades/NavigationPane>

class ApplicationUI;

class IntroScreen : public Screen<bb::cascades::NavigationPane>
{
    Q_OBJECT

    Q_PROPERTY(QString phone READ phone NOTIFY phoneChanged)
    Q_PROPERTY(bool authReady READ authReady NOTIFY authReadyChanged)
public:
    IntroScreen(ApplicationUI* app);
    ~IntroScreen();

    const QString& phone() const;
    bool authReady() const;

    Q_INVOKABLE void requestCode(const QString& phone);
    Q_INVOKABLE void requestPhoneCall();
    Q_INVOKABLE void submitCode(const QString& code);
    Q_INVOKABLE void registerUser(const QString& firstName, const QString& lastName, const QString& photo);
signals:
    void phoneChanged();
    void wrongCode();
    void authReadyChanged();
protected slots:
    void checkConnection();
protected:
    ApplicationUI*  m_app;
    QTimer          m_checkTimer;
    bool            m_authReady;

    QString         m_phone;
    bool            m_registered;
    QString         m_codeHash;
    QString         m_code;
    QString         m_firstName;
    QString         m_lastName;

    static void _requestCodeCallback(struct tgl_state *TLS, void *callback_extra, int success, int registered, const char *hash);
    static void _sendCodeCallback(struct tgl_state *TLS, void *callback_extra, int success, struct tgl_user *self);
};
