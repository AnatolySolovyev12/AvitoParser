#include "TelegramJacket.h"

TelegramJacket::TelegramJacket(QObject *parent)
	: QObject(parent)
{}


TelegramJacket::~TelegramJacket()
{}


void TelegramJacket::getUpdates()
{
	QNetworkAccessManager* manager = new QNetworkAccessManager();

	// �������� ������� ��� LongPoll (��� 0 ShortPoll) � ��������. ����� �������� offset ��� ������������� ��������� ���������� � Telegram (����� �������� �� �������� ������ ���������)
	QString urlString = QString("https://api.telegram.org/bot%1/getUpdates?offset=%2?timeout=5")
		.arg(token)
		.arg(iD);

	QNetworkRequest request(urlString);
	QNetworkReply* reply = manager->get(request);

	QObject::connect(reply, &QNetworkReply::finished, [reply, this]() {
		if (reply->error() == QNetworkReply::NoError)
		{
			QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());

			if (jsonDoc["ok"].toBool())
			{
				QJsonArray updates = jsonDoc["result"].toArray();

				for (const QJsonValue& value : updates)
				{
					QJsonObject messageObj = value["message"].toObject();
					QJsonObject fromObj = messageObj["from"].toObject();

					QString text = messageObj["text"].toString();

					qDebug() << "Received message (" << value["update_id"].toInteger() << "): " << messageObj["text"].toString() << "chatId: " << fromObj["id"].toInteger();

					iD = value["update_id"].toInteger() + 1;

					if (text == "/start")
					{
						sendMessage(chatId);
					}
				}
			}
		}
		else
		{
			qDebug() << "Error (TelegramJacket::getUpdates()): " << reply->error() << reply->errorString();
		}

		reply->deleteLater();

		});

	manager->deleteLater();
}

void TelegramJacket::sendMessage(const QString message)
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

	/*
	// ���������� ������ (���� ����������). ����������.
	QObject::connect(manager, &QNetworkAccessManager::finished, [](QNetworkReply* reply) {

		if (reply->error() == QNetworkReply::NoError)
		{
			QString response = reply->readAll();
			qDebug() << response;
		}
		else
		{
			qDebug() << "Error:: " << reply->error();
		}
		reply->deleteLater();
		});

		*/

	manager->deleteLater();
}

const QString TelegramJacket::getChatId()
{
	return chatId;
}
