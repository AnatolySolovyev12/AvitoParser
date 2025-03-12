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














/*

#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QApplication>
#include <QFile>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QList<QNetworkReply::RawHeaderPair> testListSecond;
	QList<QByteArray> testList;
	QList<QString>referenceList;

	QNetworkProxyFactory::setUseSystemConfiguration(true);
	QNetworkAccessManager nam;
	nam.setAutoDeleteReplies(true);




	QEventLoop loop;

	QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

	QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl("https://www.avito.ru/nizhnevartovsk?cd=1&q=ps5+vr2"))));

	loop.exec();

	//� ������ ������ �� ���������
	testList = reply->rawHeaderList(); // ������ ����������
	testListSecond = reply->rawHeaderPairs(); // ������ ��� �������������� ����������



	QFile file("BUFFER.txt");

	if (!(file.open(QIODevice::WriteOnly | QIODevice::Truncate))) // Truncate - ��� ������� ����������� �����
	{
		qDebug() << file.error();
	}

	QTextStream in(&file);

	in << reply->readAll() << Qt::endl;

	file.close();

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << file.error();
	}

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

	for (auto& val : referenceList)
	{
		qDebug() << val;
	}

	a.exec();

	return 0;
}

*/



/*
#include <QApplication>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QFile>
#include <qtimer.h>

void sendMessage(const QString& token, const QString& chatId, const QString& message)
{

	QNetworkAccessManager* manager = new QNetworkAccessManager();

	// ������������ URL �������
	QString urlString = QString("https://api.telegram.org/bot%1/sendMessage").arg(token);

	QUrl url(urlString);

	// ��������� ���������� ���������
	QUrlQuery query;
	query.addQueryItem("chat_id", chatId);
	query.addQueryItem("text", message);

	//qDebug() << query.toString() << "\n";


	// �������� �������
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	// �������� �������
	manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());


	// ���������� ������ (���� ����������)
	QObject::connect(manager, &QNetworkAccessManager::finished, [](QNetworkReply* reply) {

		if (reply->error() == QNetworkReply::NoError)
		{
			QString response = reply->readAll();
			//qDebug() << response;
		}
		else
		{
			qDebug() << "Error:: " << reply->error();
		}
		reply->deleteLater();
		});
}





void getUpdates(const QString& token)
{
	QNetworkAccessManager* manager = new QNetworkAccessManager();

	// ������������ URL ��� ��������� ����������
	QString urlString = QString("https://api.telegram.org/bot%1/getUpdates").arg(token);
	QUrl url(urlString);

	// �������� �������
	QNetworkRequest request(url);


	//QEventLoop loop;

	//QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

	QNetworkReply* reply = manager->get(request);

	//loop.exec();
	// �������� GET �������


	QFile file("BUFFER.txt");

	if (!(file.open(QIODevice::ReadOnly | QIODevice::Append))) // Truncate - ��� ������� ����������� �����
	{
		qDebug() << file.error();
	}

	QTextStream in(reply);

	in << reply->readAll() << Qt::endl;

	file.close();

	// ���������� ������
	QObject::connect(reply, &QNetworkReply::finished, [reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QString response = reply->readAll();
			qDebug() << "Response:" << response;



		}
		else {
			qDebug() << "Error:" << reply->error() << reply->errorString();
		}
		reply->deleteLater();
		});
}


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QString token = "7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM";
	QString chatId = "1443295378";
	QString message = "HELLOW";
	//sendMessage(token, chatId, message);

	getUpdates(token);

	QTimer timer;

	QObject::connect(&timer, &QTimer::timeout, [=]() {
		getUpdates(token);
		});
	timer.start(5000); // ��������� ������ 5 ������


	a.exec();

	return 0;
}

*/



/*

#include <QApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>
#include <QUrlQuery>

qint64 iD;

void getUpdates(const QString& token)
{
	QNetworkAccessManager* manager = new QNetworkAccessManager();

	// �������� ������� ��� LongPoll (��� 0 ShortPoll) � ��������. ����� �������� offset ��� ������������� ��������� ���������� � Telegram (����� �������� �� �������� ������ ���������)
	QString urlString = QString("https://api.telegram.org/bot%1/getUpdates?offset=%2?timeout=5")
		.arg(token)
		.arg(iD);

	//QUrl url(urlString);
	//QNetworkRequest request(url);


	QNetworkRequest request(urlString);
	QNetworkReply* reply = manager->get(request);

	QObject::connect(reply, &QNetworkReply::finished, [reply]() {
		if (reply->error() == QNetworkReply::NoError)
		{
			//qDebug() << reply->readAll();
			//QByteArray responseData = reply->readAll();
			//QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
			QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());


			if (jsonDoc["ok"].toBool())
			{
				QJsonArray updates = jsonDoc["result"].toArray();




				for (const QJsonValue& value : updates)
				{
					//QJsonObject updateObj = value.toObject();

					//if (updateObj.contains("message")) 
					//{
					QJsonObject messageObj = value["message"].toObject();

					//QString text = messageObj["text"].toString();

					//qDebug() << "Received message:" << text;
					//qDebug() << "test:" << messageObj["date"].toInt();
					//qDebug() << "test:" << messageObj["text"].toString();
					//qDebug() << "test:" << messageObj["message_id"].toInt();

					qDebug() << "Received message (" << value["update_id"].toInteger() << "): " << messageObj["text"].toString();

					iD = value["update_id"].toInteger() + 1;

					//qDebug() << "test:" << value["update_id"].toInteger();

					//}
				}
			}
			//}
		}
		else
		{
			qDebug() << "Error:" << reply->error() << reply->errorString();
		}

		reply->deleteLater();

		});
}

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);

	QString token = "7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM"; // �������� �� ��� �����


	QTimer timer;
	QObject::connect(&timer, &QTimer::timeout, [=]() {
		getUpdates(token);
		});
	timer.start(5000); // ��������� ������ 5 ������

	a.exec();

	return 0;
}

*/