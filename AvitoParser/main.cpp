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

	QString urlString = "https://www.avito.ru/nizhnevartovsk/igry_pristavki_i_programmy/igry_dlya_pristavok-ASgBAgICAUSSAsYJ?cd=1&q=%D0%B8%D0%B3%D1%80%D1%8B+%D0%BD%D0%B0+playstation+5";

	int page = 1;

	for (int val = 1; val <= 4; val++)
	{
		QString temporaryUrl = urlString.insert((urlString.indexOf("cd=1") + 4), "&p=" + QString::number(page));

		QEventLoop loop;

		QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

		//QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl("https://www.avito.ru/nizhnevartovsk?cd=1&q=ps5+vr2"))));
		QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl(temporaryUrl))));

		loop.exec();

		//в данный момент не требуется
		//testList = reply->rawHeaderList(); // список заголовков
		//testListSecond = reply->rawHeaderPairs(); // список пар необработанных заголовков

		QFile file("BUFFER.txt");

		if (!(file.open(QIODevice::ReadWrite | QIODevice::Append))) // Append - для добавления содержимого в файла
		//if (!(file.open(QIODevice::ReadWrite | QIODevice::Truncate))) // Truncate - для очистки содержимого файла
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
			{
				int index = line.indexOf("/nizhnevartovsk/igry_pristavki_i_programmy/");

				QString temporary;

				for (auto& val : line.sliced(index))
				{
					if (val == ')' || val == ' ' || val == '?')
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

		page++;
	}

	for (auto& val : referenceList)
	{
		qDebug() << val;
	}

	qDebug() << "\n" << "Count of reference: " + QString::number(referenceList.length());

	a.exec();
	
	return 0;
}