#include "AvitoParser.h"
#include <QtWidgets/QApplication>
#include <windows.h>

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(1251);  // Установить UTF-8

	QApplication a(argc, argv);

	AvitoParser w;
	w.show();
	w.setWindowIcon(QIcon("icon.png"));
	w.setWindowTitle("AvitoParser by Solovev");

	a.exec();

	return 0;
}
