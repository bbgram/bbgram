#include "timers.h"
#include "../Telegraph.h"


void tgl_timer::onTimeout()
{
    callback(TLS, arg);
}


struct tgl_timer * alloc_timer(struct tgl_state *TLS, void (*cb)(struct tgl_state *TLS, void *arg), void *arg)
{
    tgl_timer* timer = new tgl_timer();
    timer->setParent(Telegraph::instance());
    timer->TLS = TLS;
    timer->callback = cb;
    timer->arg = arg;
    return timer;
}

void insert_timer(struct tgl_timer *t, double timeout)
{
    QObject::connect(t, SIGNAL(timeout()), t, SLOT(onTimeout()));
    t->setInterval((int)(timeout * 1000));
    t->setSingleShot(true);
    t->start();
}

void remove_timer(struct tgl_timer *t)
{
    t->stop();
}

void free_timer(struct tgl_timer *t)
{
    delete t;
}
