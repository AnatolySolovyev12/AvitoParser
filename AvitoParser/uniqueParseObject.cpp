#include "uniqueParseObject.h"

uniqueParseObject::uniqueParseObject(QObject *parent)
	: QObject(parent)
{






}

uniqueParseObject::~uniqueParseObject()
{}




void uniqueParseObject::generalParseFunc()
{
	//QList<QNetworkReply::RawHeaderPair> testListSecond;
	//QList<QByteArray> testList;

	QList<QString>referenceList;

	QNetworkProxyFactory::setUseSystemConfiguration(true);
	QNetworkAccessManager nam;
	nam.setAutoDeleteReplies(true);

	//QString urlString = "https://www.avito.ru/nizhnevartovsk/igry_pristavki_i_programmy/igry_dlya_pristavok-ASgBAgICAUSSAsYJ?cd=1&q=%D0%B8%D0%B3%D1%80%D1%8B+%D0%BD%D0%B0+playstation+5"; // обычный запрос
	QString urlString = "https://www.avito.ru/nizhnevartovsk/igry_pristavki_i_programmy/igry_dlya_pristavok-ASgBAgICAUSSAsYJ?cd=1&q=%D0%B8%D0%B3%D1%80%D1%8B+%D0%BD%D0%B0+playstation+5&s=104"; // обычный с сортировкой по дате

	//int page = 1;// при многостраничном поиске
	//for (int val = 1; val <= 4; val++) // при многостраничном поиске
		//QString temporaryUrl = urlString;// при многостраничном поиске
		//temporaryUrl.insert((urlString.indexOf("cd=1") + 4), "&p=" + QString::number(page));// при многостраничном поиске

	qDebug() << "\n" << urlString << "\n";

	QEventLoop loop;

	QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

	QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl(urlString))));

	loop.exec();

	//в данный момент не требуется
	//testList = reply->rawHeaderList(); // список заголовков
	//testListSecond = reply->rawHeaderPairs(); // список пар необработанных заголовков

	//QFile file("BUFFER" + QString::number(page) + ".txt"); // при многостраничном поиске создание нескольких файлов
	QFile file("BUFFER.txt");

	if (!(file.open(QIODevice::ReadWrite | QIODevice::Truncate))) // Truncate - для очистки содержимого файла
		//if (!(file.open(QIODevice::ReadWrite | QIODevice::Append))) // Append - для добавления содержимого в файла
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

	//page++;// при многостраничном поиске

	for (auto& val : referenceList)
	{
		qDebug() << val;
	}

	qDebug() << "\n" << "Count of reference: " + QString::number(referenceList.length());
}