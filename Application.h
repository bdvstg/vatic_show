#ifndef __AUTOAVM_APPLICATION_H__
#define __AUTOAVM_APPLICATION_H__

#include <QApplication>

class Application: public QApplication
{
public:
    Application(int argc, char * argv[]);
    bool notify(QObject * receiver, QEvent * event);
};

#endif // __AUTOAVM_APPLICATION_H__
