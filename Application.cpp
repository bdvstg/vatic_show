#include "Application.h"
#include <QMessageBox>

Application::Application(int argc, char * argv[])
    : QApplication(argc, argv)
{
}

bool Application::notify(QObject * receiver, QEvent * event)
{
    bool retVal = false;

    try {
        retVal = QApplication::notify(receiver, event);
    } catch (std::exception & e) {
        QMessageBox::critical(nullptr, "Error", e.what());
    }

    return retVal;
}
