#pragma once

#include <QTimer.h>
extern "C" {
#include <tgl.h>
}

class tgl_timer : public QTimer
{
    Q_OBJECT
public:
    static struct tgl_timer * alloc(struct tgl_state *TLS, void (*cb)(struct tgl_state *TLS, void *arg), void *arg);
    static void insert(struct tgl_timer *t, double timeout);
    static void remove(struct tgl_timer *t);
    static void free(struct tgl_timer *t);
public slots:
    void onTimeout();

protected:
    struct tgl_state *m_TLS;
    void (*m_callback)(struct tgl_state *, void *);
    void *m_arg;
};
