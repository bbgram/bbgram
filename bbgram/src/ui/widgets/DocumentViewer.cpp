#include "DocumentViewer.h"

#include <QtGlobal>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Container>

using namespace bb::cascades;

DocumentViewer::DocumentViewer()
: m_message(0)
{
    QmlDocument *qml = QmlDocument::create("asset:///ui/pages/chats/messages/Document.qml").parent(this);
    m_root = qml->createRootObject<Container>();
    setRoot(m_root);
}

DocumentViewer::~DocumentViewer()
{
}

Message* DocumentViewer::message() const
{
    return m_message;
}

QString readableFileSize(int size)
{
    const char* units[] = {"B", "KB", "MB", "GB"};

    float sz = size;
    int i = 0;
    while (sz > 1024) {
        sz /= 1024.f;
        i++;
    }
    return QString("%1 %2").arg(QString::number(qMax(sz, 0.1f), 'f', 1)).arg(QString::fromUtf8(units[i]));
}

void DocumentViewer::setMessage(Message *message)
{
    m_message = message;

    const QVariantMap& media = message->media();
    m_root->setProperty("incoming", !message->our());

    QString s = media["caption"].toString();
    if (s.length() > 0)
        m_root->setProperty("caption", s);

    int sz = media["size"].toInt();
    m_root->setProperty("fileSize", readableFileSize(sz));
}
