#include "AvitoParser.h"

#include <QtWidgets/QApplication>

#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QFile>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	AvitoParser w;
	w.show();

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

	//в данный момент не требуется
	testList = reply->rawHeaderList(); // список заголовков
	testListSecond = reply->rawHeaderPairs(); // список пар необработанных заголовков



	QFile file("BUFFER.txt");

	if (!(file.open(QIODevice::WriteOnly | QIODevice::Truncate))) // Truncate - для очистки содержимого файла
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

	// Формирование URL запроса
	QString urlString = QString("https://api.telegram.org/bot%1/sendMessage").arg(token);

	QUrl url(urlString);

	// Настройка параметров сообщения
	QUrlQuery query;
	query.addQueryItem("chat_id", chatId);
	query.addQueryItem("text", message);

	//qDebug() << query.toString() << "\n";


	// Создание запроса
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	// Отправка запроса
	manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());


	// Обработчик ответа (если необходимо)
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

	// Формирование URL для получения обновлений
	QString urlString = QString("https://api.telegram.org/bot%1/getUpdates").arg(token);
	QUrl url(urlString);

	// Создание запроса
	QNetworkRequest request(url);


	//QEventLoop loop;

	//QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

	QNetworkReply* reply = manager->get(request);

	//loop.exec();
	// Отправка GET запроса


	QFile file("BUFFER.txt");

	if (!(file.open(QIODevice::ReadOnly | QIODevice::Append))) // Truncate - для очистки содержимого файла
	{
		qDebug() << file.error();
	}

	QTextStream in(reply);

	in << reply->readAll() << Qt::endl;

	file.close();

	// Обработчик ответа
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
	timer.start(5000); // Проверяем каждые 5 секунд


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

	// добавлен таймаут для LongPoll (при 0 ShortPoll) в секундах. Также добавлен offset для подтверждения получения сообщдения в Telegram (чтобы повторно не поулчать старые сообщения)
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

	QString token = "7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM"; // Замените на ваш токен


	QTimer timer;
	QObject::connect(&timer, &QTimer::timeout, [=]() {
		getUpdates(token);
		});
	timer.start(5000); // Проверяем каждые 5 секунд

	a.exec();

	return 0;
}

*/