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
    void mainDCAuthorized();
    void allDCAuthorized();

    void ready();
protected slots:
    void checkAllAuthorized();
    void writeState();
protected:
    static Telegraph* m_instance;

    tgl_update_callback m_updateCallbacks;

    QTimer* m_checkAllAuthorizedTimer;
    QTimer* m_writeStateTimer;
};
