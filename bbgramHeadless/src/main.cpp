#include <bb/Application>
#include "HeadlessApplication.hpp"

using bb::Application;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    new HeadlessApplication(&app);

    return Application::exec();
}
