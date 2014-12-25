#include "net.h"

#include "../Telegraph.h"

#include <assert.h>
#include <time.h>
#include <QDebug>

#define PING_TIMEOUT 10

void connection::restartConnection()
{
    if (m_lastConnectTime == time(0))
    {
        QTimer::singleShot(10*1000, this, SLOT(restartConnection()));
        return;
    }
    disconnectFromHost();
}

void connection::failConnection()
{
    if (m_state == conn_ready)
    {
        m_pingTimer->stop();
    }

    qDebug() << "Lost connection to server... " << m_host << ":" << m_port;
    m_state = conn_failed;

    rotatePort();
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
    if (tglt_get_double_time () - m_lastReceiveTime > 6 * PING_TIMEOUT)
    {
        //vlogprintf(E_WARNING, "fail connection: reason: ping timeout\n");
        failConnection();
    }
    else if (tglt_get_double_time () - m_lastReceiveTime > 3 * PING_TIMEOUT && m_state == conn_ready)
    {
        tgl_do_send_ping(m_TLS, this);
    }
}

void connection::onConnected()
{
    char byte = 0xef;
    assert (connection::write_out(this, &byte, 1) == 1);

    m_lastReceiveTime = tglt_get_double_time();
    m_pingTimer->start();

    m_state = conn_ready;
    m_methods->ready(m_TLS, this);
}

void connection::onDisconnected()
{
    close();
    m_lastConnectTime = time(0);
    connectToHost(m_host, m_port);
}


void connection::tryRpcRead()
{
    forever
    {
        if (m_buffer.length() < 1)
            return;

        unsigned len = 0;

        assert (connection::read_in_lookup (this, &len, 1) == 1);
        if (len >= 1 && len <= 0x7e)
        {
            if (m_buffer.length() < (int)(1 + 4 * len))
                return;
        }
        else
        {
            if (m_buffer.length() < 4)
                return;

            assert (connection::read_in_lookup (this, &len, 4) == 4);
            len = (len >> 8);
            if (m_buffer.length() < (int)(4 + 4 * len))
                return;

            len = 0x7f;
        }

        unsigned t = 0;
        if (len >= 1 && len <= 0x7e)
        {
            assert (connection::read_in (this, &t, 1) == 1);
            assert (t == len);
            assert (len >= 1);
        }
        else
        {
            assert (len == 0x7f);
            assert (connection::read_in (this, &len, 4) == 4);
            len = (len >> 8);
            assert (len >= 1);
        }
        len *= 4;
        int op;
        assert (connection::read_in_lookup (this, &op, 4) == 4);
        if (m_methods->execute (m_TLS, this, op, len) < 0)
            return;
    }
}


void connection::onReadyRead()
{
    m_lastReceiveTime = tglt_get_double_time();
    m_pingTimer->stop();
    m_pingTimer->start();

    QByteArray bytes = readAll();
    m_buffer.append(bytes);
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


struct connection *connection::create(struct tgl_state *TLS, const char *host, int port, struct tgl_session *session, struct tgl_dc *dc, struct mtproto_methods *methods)
{
    connection* conn = new connection();
    conn->setParent(Telegraph::instance());

    conn->m_TLS = TLS;
    conn->m_host = (char*)malloc(strlen(host)+1);
    strcpy(conn->m_host, host);
    conn->m_port = port;
    conn->m_session = session;
    conn->m_dc = dc;
    conn->m_methods = methods;

    conn->m_lastReceiveTime = tglt_get_double_time();
    conn->m_outPacketNum = 0;
    conn->m_state = conn_connecting;

    QObject::connect(conn, SIGNAL(connected()), conn, SLOT(onConnected()));
    QObject::connect(conn, SIGNAL(disconnected()), conn, SLOT(onDisconnected()));
    QObject::connect(conn, SIGNAL(readyRead()), conn, SLOT(onReadyRead()));
    QObject::connect(conn, SIGNAL(error(QAbstractSocket::SocketError)),
            conn, SLOT(onError(QAbstractSocket::SocketError)));

    conn->m_lastConnectTime = time(0);
    conn->connectToHost(host, port);

    return conn;
}

int connection::write_out(struct connection *conn, const void *data, int len)
{
    //if (((QAbstractSocket*)c)->state() != QAbstractSocket::ConnectedState)
    //    return 0;

    qint64 written = conn->write((const char*)data, (qint64)len);
    return (int)written;
}


int connection::read_in(struct connection *conn, void *data, int len)
{
    if (!len || !conn->m_buffer.length())
        return 0;
    assert (len > 0);
    if (len > conn->m_buffer.length())
        len = conn->m_buffer.length();
    memcpy(data, conn->m_buffer.data(), len);
    conn->m_buffer.remove(0, len);
    return len;
}

int connection::read_in_lookup(struct connection *conn, void *data, int len)
{
    if (!len || !conn->m_buffer.length())
        return 0;
    assert (len > 0);
    if (len > conn->m_buffer.length())
      len = conn->m_buffer.length();

    memcpy(data, conn->m_buffer.data(), len);
    return len;
}

void connection::flush_out(struct connection *conn)
{
    conn->flush();
}

void connection::incr_out_packet_num(struct connection *conn)
{
    conn->m_outPacketNum++;
}

void connection::free(struct connection *conn)
{
    ::free(conn->m_host);
    delete conn;
}

struct tgl_dc *connection::get_dc(struct connection *conn)
{
    return conn->m_dc;
}

struct tgl_session *connection::get_session(struct connection *conn)
{
    return conn->m_session;
}

void connection::rotatePort()
{
    switch (m_port)
    {
        case 443:
            m_port = 80;
            break;
        case 80:
            m_port = 25;
            break;
        case 25:
            m_port = 443;
            break;
    }
}
