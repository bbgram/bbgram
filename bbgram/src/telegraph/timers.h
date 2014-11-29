#pragma once

#include <QTimer.h>
extern "C" {
#include <tgl.h>
}

class tgl_timer : public QTimer
{
    Q_OBJECT
public slots:
    void onTimeout();
public:
    struct tgl_state *TLS;
    void (*callback)(struct tgl_state *, void *);
    void *arg;
};

struct tgl_timer * alloc_timer(struct tgl_state *TLS, void (*cb)(struct tgl_state *TLS, void *arg), void *arg);

void insert_timer(struct tgl_timer *t, double timeout);
void remove_timer(struct tgl_timer *t);
void free_timer(struct tgl_timer *t);
