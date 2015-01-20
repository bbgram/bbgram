#pragma once

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>

class Telegraph : public QObject
{
    Q_OBJECT
public:
    Telegraph();
    ~Telegraph();

    static Telegraph* instance();
public slots:
    void start();
    void stop();

    void exportAuthorization();

    void onReady();
signals:
    void ready();
protected slots:
    void writeState();
protected:
    static Telegraph* m_instance;
    static void onLoginIn(struct tgl_state *TLS);

    tgl_update_callback m_updateCallbacks;
    tgl_timer_methods   m_timerMethods;
    tgl_net_methods     m_netMethods;

    QTimer* m_writeStateTimer;
};
