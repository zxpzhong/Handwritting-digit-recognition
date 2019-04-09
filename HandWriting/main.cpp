#include "HandWriting.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HandWriting w;
    w.show();
    return a.exec();
}
