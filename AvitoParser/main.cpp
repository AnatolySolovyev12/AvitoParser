#include "AvitoParser.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AvitoParser w;
    w.show();
    return a.exec();
}
