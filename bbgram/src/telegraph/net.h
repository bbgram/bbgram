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
    static struct connection *create(struct tgl_state *TLS, const char *host, int port, struct tgl_session *session, struct tgl_dc *dc, struct mtproto_methods *methods);
    static int write_out(struct connection *c, const void *data, int len);
    static int read_in(struct connection *c, void *data, int len);
    static int read_in_lookup(struct connection *c, void *data, int len);
    static void flush_out(struct connection *c);
    static void incr_out_packet_num(struct connection *c);
    static void free(struct connection *c);
    static struct tgl_dc *get_dc(struct connection *c);
    static struct tgl_session *get_session(struct connection *c);
protected:
    struct tgl_state *m_TLS;
    char *m_host;
    int m_port;
    struct tgl_session *m_session;
    struct tgl_dc *m_dc;
    struct mtproto_methods *m_methods;

    int m_lastConnectTime;
    double m_lastReceiveTime;
    int m_outPacketNum;

    QByteArray m_buffer;
    QList<QByteArray>   m_writeBuffers;
    qint64              m_bufferOffset;
    QTimer* m_pingTimer;

    void rotatePort();
    void writeBuffer();
protected slots:
    void restartConnection();
    void tryRpcRead();
    void bytesWritten(qint64 bytes);
};
