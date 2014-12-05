#include "timers.h"
#include "../Telegraph.h"


void tgl_timer::onTimeout()
{
    m_callback(m_TLS, m_arg);
}


struct tgl_timer * tgl_timer::alloc(struct tgl_state *TLS, void (*cb)(struct tgl_state *TLS, void *arg), void *arg)
{
    tgl_timer* timer = new tgl_timer();
    timer->setParent(Telegraph::instance());
    timer->m_TLS = TLS;
    timer->m_callback = cb;
    timer->m_arg = arg;
    return timer;
}

void tgl_timer::insert(struct tgl_timer *t, double timeout)
{
    QObject::connect(t, SIGNAL(timeout()), t, SLOT(onTimeout()));
    t->setInterval((int)(timeout * 1000));
    t->setSingleShot(true);
    t->start();
}

void tgl_timer::remove(struct tgl_timer *t)
{
    t->stop();
}

void tgl_timer::free(struct tgl_timer *t)
{
    delete t;
}
