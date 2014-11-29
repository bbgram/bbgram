#pragma once

extern "C" {
#include <tgl.h>
}
#include <QTimer>
#include <QtNetwork/QTcpSocket>

enum conn_state {
  conn_none,
  conn_connecting,
  conn_ready,
  conn_failed,
  conn_stopped
};

class connection : public QTcpSocket
{
    Q_OBJECT
public:
    connection();
    ~connection();
public slots:
    void checkPingTimeout();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
public:
    struct tgl_state *TLS;
    char *host;
    int port;
    struct tgl_session *session;
    struct tgl_dc *dc;
    struct mtproto_methods *methods;

    int last_connect_time;
    double last_receive_time;
    int out_packet_num;
    conn_state state;

    QByteArray buffer;
protected:
    QTimer* m_pingTimer;

protected slots:
    void failConnection();
    void restartConnection();
    void tryRpcRead();
};

struct connection *connection_create(struct tgl_state *TLS, const char *host, int port, struct tgl_session *session, struct tgl_dc *dc, struct mtproto_methods *methods);
int connection_write_out(struct connection *c, const void *data, int len);
int connection_read_in(struct connection *c, void *data, int len);
int connection_read_in_lookup(struct connection *c, void *data, int len);
void connection_flush_out(struct connection *c);
void connection_incr_out_packet_num(struct connection *c);
void connection_free(struct connection *c);
struct tgl_dc *connection_get_dc(struct connection *c);
struct tgl_session *connection_get_session(struct connection *c);
