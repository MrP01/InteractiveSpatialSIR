#include <QtCore/QCoreApplication>

#include "Server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server(1234);
    return a.exec();
}