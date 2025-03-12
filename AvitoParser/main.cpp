#include "AvitoParser.h"
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QtWidgets/QApplication>
#include <QFile>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	//AvitoParser w;
	//w.show();

	//QList<QNetworkReply::RawHeaderPair> testListSecond;
	//QList<QByteArray> testList;

	QList<QString>referenceList;

	QNetworkProxyFactory::setUseSystemConfiguration(true);
	QNetworkAccessManager nam;
	nam.setAutoDeleteReplies(true);

	//QString urlString = "https://www.avito.ru/nizhnevartovsk/igry_pristavki_i_programmy/igry_dlya_pristavok-ASgBAgICAUSSAsYJ?cd=1&q=%D0%B8%D0%B3%D1%80%D1%8B+%D0%BD%D0%B0+playstation+5"; // ������� ������
	QString urlString = "https://www.avito.ru/nizhnevartovsk/igry_pristavki_i_programmy/igry_dlya_pristavok-ASgBAgICAUSSAsYJ?cd=1&q=%D0%B8%D0%B3%D1%80%D1%8B+%D0%BD%D0%B0+playstation+5&s=104"; // ������� � ����������� �� ����
	//QString urlString = "https://www.avito.ru/nizhnevartovsk/igry_pristavki_i_programmy/igry_dlya_pristavok-ASgBAgICAUSSAsYJ?q=%D0%B8%D0%B3%D1%80%D1%8B+%D0%BD%D0%B0+playstation+5+VR2&s=104"; // ������� � ����������� �� ����
	
	//QString urlString = "https://www.avito.ru/nizhnevartovsk/telefony/mobilnye_telefony/huawei-ASgBAgICAkS0wA3wrjmwwQ2I_Dc?q=%D1%82%D0%B5%D0%BB%D0%B5%D1%84%D0%BE%D0%BD+Huawei+p&s=104"; // ������� � ����������� �� ����

	//int page = 1;// ��� ��������������� ������
	//for (int val = 1; val <= 4; val++) // ��� ��������������� ������
		//QString temporaryUrl = urlString;// ��� ��������������� ������
		//temporaryUrl.insert((urlString.indexOf("cd=1") + 4), "&p=" + QString::number(page));// ��� ��������������� ������

	qDebug() << "\n" << urlString << "\n";

	QEventLoop loop;

	QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

	QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl(urlString))));

	loop.exec();

	//� ������ ������ �� ���������
	//testList = reply->rawHeaderList(); // ������ ����������
	//testListSecond = reply->rawHeaderPairs(); // ������ ��� �������������� ����������

	//QFile file("BUFFER" + QString::number(page) + ".txt");
	QFile file("BUFFER.txt");

	if (!(file.open(QIODevice::ReadWrite | QIODevice::Truncate))) // Truncate - ��� ������� ����������� �����
		//if (!(file.open(QIODevice::ReadWrite | QIODevice::Append))) // Append - ��� ���������� ����������� � �����
	{
		qDebug() << file.error();
	}

	QTextStream in(&file);

	in << reply->readAll() << Qt::endl;

	in.seek(0);

	while (!in.atEnd())
	{
		QString line = in.readLine();

		if (line.indexOf("/nizhnevartovsk/igry_pristavki_i_programmy/") != -1)
		//if (line.indexOf("/nizhnevartovsk/telefony/") != -1)
		{
			int index = line.indexOf("/nizhnevartovsk/igry_pristavki_i_programmy/");
			//int index = line.indexOf("/nizhnevartovsk/telefony/");

			QString temporary;

			for (QString val : line.sliced(index))
			{
				if (val == ')' || val == ' ' || val == '?' || val == '&' || val == '"')
				{
					break;
				}

				temporary += val;
			}

			temporary.push_front("https://www.avito.ru");

			if (referenceList.indexOf(temporary) == -1)
			{
				referenceList.push_back(temporary);
			}
		}
	}

	file.close();

	//page++;// ��� ��������������� ������


	for (auto& val : referenceList)
	{
		qDebug() << val;
	}

	qDebug() << "\n" << "Count of reference: " + QString::number(referenceList.length());


	a.exec();

	return 0;
}