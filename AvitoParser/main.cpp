#include "AvitoParser.h"
#include <QtWidgets/QApplication>
#include <windows.h>

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(1251);  // Установить UTF-8

	QApplication a(argc, argv);

	AvitoParser w;
    w.show();


	QString str = "Привет, мир!";

		//QString str = QString::fromUtf8("Привет, мир!");
		//std::string stdStr = "Привет, мир!";
	//QString str = QString::fromStdString(stdStr);

		//QString str = QStringLiteral("Привет, мир!");





	a.exec();

	return 0;
}
