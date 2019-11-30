#include "gameclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameClient cl;
    cl.show();

    return a.exec();
}
