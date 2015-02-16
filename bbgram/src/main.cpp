#include <bb/cascades/Application>
#include <QLocale>

#include "ApplicationUi.h"

void* TAILQ_END() { return 0; }

#include "UDSWrapper.h"


Q_DECL_EXPORT int main(int argc, char **argv)
{
    QLocale::setDefault(QLocale::English);

    QCoreApplication::setOrganizationName("Big Boss");
    QCoreApplication::setApplicationName("bomogram");

    bb::cascades::Application app(argc, argv);

    UDSWrapper::initialize();

    ApplicationUI appui(&app);
    int result = bb::cascades::Application::exec();

    return result;
}
