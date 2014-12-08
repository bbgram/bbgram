#include "net.h"

#include "../Telegraph.h"

#include <assert.h>
#include <time.h>
#include <QDebug>

#define PING_TIMEOUT 10

static void rotate_port (struct connection *c)
{
    switch (c->port)
    {
        case 443:
            c->port = 80;
            break;
        case 80:
            c->port = 25;
            break;
        case 25:
            c->port = 443;
            break;
    }
}

void connection::restartConnection()
{
    if (last_connect_time == time(0))
    {
        QTimer::singleShot(10*1000, this, SLOT(restartConnection()));
        return;
    }
    disconnectFromHost();
}

void connection::failConnection()
{
    if (state == conn_ready)
    {
        m_pingTimer->stop();
    }

    qDebug() << "Lost connection to server... " << host << ":" << port;
    state = conn_failed;
    rotate_port (this);

    restartConnection();
}

connection::connection()
{
    m_pingTimer = new QTimer(this);
    m_pingTimer->setInterval(PING_TIMEOUT * 1000);
    connect(m_pingTimer, SIGNAL(timeout()), this, SLOT(checkPingTimeout()));
}

connection::~connection()
{
}

void connection::checkPingTimeout()
{
    if (tglt_get_double_time () - last_receive_time > 6 * PING_TIMEOUT)
    {
        //vlogprintf(E_WARNING, "fail connection: reason: ping timeout\n");
        failConnection();
    }
    else if (tglt_get_double_time () - last_receive_time > 3 * PING_TIMEOUT && state == conn_ready)
    {
        tgl_do_send_ping(TLS, this);
    }
}

void connection::onConnected()
{
    char byte = 0xef;
    assert (connection_write_out(this, &byte, 1) == 1);

    last_receive_time = tglt_get_double_time();
    m_pingTimer->start();

    state = conn_ready;
    methods->ready(TLS, this);
}

void connection::onDisconnected()
{
    close();
    last_connect_time = time(0);
    connectToHost(host, port);
}


void connection::tryRpcRead()
{
    forever
    {
        if (buffer.length() < 1)
            return;

        unsigned len = 0;

        assert (connection_read_in_lookup (this, &len, 1) == 1);
        if (len >= 1 && len <= 0x7e)
        {
            if (buffer.length() < (int)(1 + 4 * len))
                return;
        }
        else
        {
            if (buffer.length() < 4)
                return;

            assert (connection_read_in_lookup (this, &len, 4) == 4);
            len = (len >> 8);
            if (buffer.length() < (int)(4 + 4 * len))
                return;

            len = 0x7f;
        }

        unsigned t = 0;
        if (len >= 1 && len <= 0x7e)
        {
            assert (connection_read_in (this, &t, 1) == 1);
            assert (t == len);
            assert (len >= 1);
        }
        else
        {
            assert (len == 0x7f);
            assert (connection_read_in (this, &len, 4) == 4);
            len = (len >> 8);
            assert (len >= 1);
        }
        len *= 4;
        int op;
        assert (connection_read_in_lookup (this, &op, 4) == 4);
        if (this->methods->execute (TLS, this, op, len) < 0)
            return;
    }
}


void connection::onReadyRead()
{
    last_receive_time = tglt_get_double_time();
    m_pingTimer->stop();
    m_pingTimer->start();

    QByteArray bytes = readAll();
    buffer.append(bytes);
    tryRpcRead();
}

void connection::onError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError)
     {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
         qDebug() << "The host was not found";
         break;
     case QAbstractSocket::ConnectionRefusedError:
         qDebug() << "The connection was refused by the peer";
         break;
     default:
         qDebug() << "tcperr: " << errorString();
     }
 }


struct connection *connection_create(struct tgl_state *TLS, const char *host, int port, struct tgl_session *session, struct tgl_dc *dc, struct mtproto_methods *methods)
{
    connection* conn = new connection();
    conn->setParent(Telegraph::instance());

    conn->TLS = TLS;
    conn->host = (char*)malloc(strlen(host)+1);
    strcpy(conn->host, host);
    conn->port = port;
    conn->session = session;
    conn->dc = dc;
    conn->methods = methods;

    conn->last_receive_time = tglt_get_double_time();
    conn->out_packet_num = 0;
    conn-> state = conn_connecting;

    QObject::connect(conn, SIGNAL(connected()), conn, SLOT(onConnected()));
    QObject::connect(conn, SIGNAL(disconnected()), conn, SLOT(onDisconnected()));
    QObject::connect(conn, SIGNAL(readyRead()), conn, SLOT(onReadyRead()));
    QObject::connect(conn, SIGNAL(error(QAbstractSocket::SocketError)),
            conn, SLOT(onError(QAbstractSocket::SocketError)));

    conn->last_connect_time = time(0);
    conn->connectToHost(host, port);

    return conn;
}

int connection_write_out(struct connection *c, const void *data, int len)
{
    //if (((QAbstractSocket*)c)->state() != QAbstractSocket::ConnectedState)
    //    return 0;

    qint64 written = c->write((const char*)data, (qint64)len);
    return (int)written;
}


int connection_read_in(struct connection *c, void *data, int len)
{
    if (!len || !c->buffer.length())
        return 0;
    assert (len > 0);
    if (len > c->buffer.length())
        len = c->buffer.length();
    memcpy(data, c->buffer.data(), len);
    c->buffer.remove(0, len);
    return len;
}

int connection_read_in_lookup(struct connection *c, void *data, int len)
{
    if (!len || !c->buffer.length())
        return 0;
    assert (len > 0);
    if (len > c->buffer.length())
      len = c->buffer.length();

    memcpy(data, c->buffer.data(), len);
    return len;
}

void connection_flush_out(struct connection *c)
{
    c->flush();
}

void connection_incr_out_packet_num(struct connection *c)
{
    c->out_packet_num++;
}

void connection_free(struct connection *c)
{
    free(c->host);
    delete c;
}

struct tgl_dc *connection_get_dc(struct connection *c)
{
    return c->dc;
}

struct tgl_session *connection_get_session(struct connection *c)
{
    return c->session;
}

