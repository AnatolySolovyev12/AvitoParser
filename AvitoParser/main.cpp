#include "AvitoParser.h"
#include <QtWidgets/QApplication>
#include <windows.h>

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(1251);  // Установить UTF-8

	QApplication a(argc, argv);

	AvitoParser w;
    w.show();

	a.exec();

	return 0;
}
