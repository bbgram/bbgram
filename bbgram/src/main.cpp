#include <bb/cascades/Application>
#include <QLocale>

#include "ApplicationUi.h"

void* TAILQ_END() { return 0; }


Q_DECL_EXPORT int main(int argc, char **argv)
{
    QLocale::setDefault(QLocale::English);

    bb::cascades::Application app(argc, argv);

    //QThread *thread = new QThread;
    /*Telegraph* telegraph = new Telegraph;
    telegraph->moveToThread(thread);
    QObject::connect(thread, SIGNAL(started()), telegraph, SLOT(start()));
    QObject::connect(telegraph, SIGNAL(finished()), thread, SLOT(quit()) );

    QObject::connect(thread, SIGNAL(finished()), telegraph, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();*/

    ApplicationUI appui(&app);
    int result = bb::cascades::Application::exec();

    /*telegraph->stop();
    QMetaObject::invokeMethod(telegraph, "stop", Qt::QueuedConnection);
    if(!thread->wait(1000))
    {
        thread->terminate();
        thread->wait();
    }*/

    return result;
}
