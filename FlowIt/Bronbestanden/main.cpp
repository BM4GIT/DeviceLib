#include "flowit.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FlowIt w;
    w.show();

    return a.exec();
}
