#include "uniqueParseObject.h"

uniqueParseObject::uniqueParseObject(QObject* parent)
	: QObject(parent), classTimer(new QTimer())
{
	connect(classTimer, &QTimer::timeout, this, &uniqueParseObject::classTimerIsDone);
	classTimer->stop();

	 prefix = R"(data-marker="item-title" href=")";
	 stampPrefix = R"(data-marker="item-date">)";
}


void uniqueParseObject::generalParseFunc()
{
	//qDebug() << m_name << " is DONE"; // для дебага

	try {
		if (!QUrl(m_URL).isValid()) // проверка соответствия адреса на корректность
		{
			qWarning() << "Invalid URL:" << m_URL;
			return;
		}

		if (firstAccumulateReferenceValue > 0)
			--firstAccumulateReferenceValue;

		QNetworkProxyFactory::setUseSystemConfiguration(true);
		QNetworkAccessManager nam;

		QEventLoop loop;

		QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

		QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl(m_URL))));

		if (!reply) {
			throw std::runtime_error("Failed to create network request");
		}

		QTimer::singleShot(20000, &loop, &QEventLoop::quit);

		loop.exec();

		if (reply->error() != QNetworkReply::NoError) {
			throw std::runtime_error(reply->errorString().toStdString());
		}

		fileParseFunc(reply->readAll());

		if (countOfReference < referenceList.length())
		{
			QTimer::singleShot(1000, [this](){

				qDebug() << "\n" << QDateTime::currentDateTime().toString() << "(" + m_name + ")" + " count of reference: " + QString::number(referenceList.length()) << '\n';
				countOfReference = int(referenceList.length());
				});
		}

	}
	catch (const std::exception& e) {
		qWarning() << "Error in uniqueParseObject::generalParseFunc:" << e.what();
	}
}


void uniqueParseObject::fileParseFunc(const QByteArray& data)
{
	QFile file("BUFFER_" + m_name + ".txt"); // при многостраничном поиске создание нескольких файлов

	if (!(file.open(QIODevice::ReadWrite | QIODevice::Truncate))) // Truncate - для очистки содержимого файла
		//if (!(file.open(QIODevice::ReadWrite | QIODevice::Append))) // Append - для добавления содержимого в файла
	{
		qWarning() << "Error in uniqueParseObject::fileParseFunc:" << file.error();
	}

	QFile fileRef(m_name + "_RefMassive.txt");

	if (!(fileRef.open(QIODevice::WriteOnly | QIODevice::Append))) // Truncate - для очистки содержимого файла
	{
		qWarning() << "Error: (addInRefMassive) " + m_name + "_RefMassive.txt" << fileRef.error();
		return;
	}

	QTextStream inRef(&fileRef);

	QTextStream in(&file);

	in << data << Qt::endl;

	in.seek(0);

	int countterOfEntre = 0; // ограничиваемся первыми 30-ю записями для фильтрации

	while (!in.atEnd())
	{
		QString line = in.readLine();

		if (line.indexOf(prefix) != -1)
		{
			if (countterOfEntre >= 6)
				break;

			countterOfEntre++;

			int index = line.indexOf(prefix) + prefix.length();

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
				inRef << temporary << '\n';
				potentialNewString = temporary;
			}
		}
		
		if (line.indexOf(stampPrefix) != -1 && potentialNewString != "")
		{
			int index = line.indexOf(stampPrefix) + stampPrefix.length();

			QString timeStamp;

			for (QString val : line.sliced(index))
			{
				if (val == '<')
				{
					break;
				}

				timeStamp += val;
			}

			qDebug() << potentialNewString << "   " << timeStamp;

			if (timeStamp == "1 час назад") // берем самые актуальные записи
			{
				if (firstAccumulateReferenceValue == 0) // приступаем к отправке после аккумуляции некоторой части записей
				{
					emit messageReceived(potentialNewString);
					potentialNewString = "";
				}
			}

			potentialNewString = "";
		}
	}

	file.close();

	fileRef.close();
}


void uniqueParseObject::setParam(QString name, QString URL, QString updateSecond, bool checkParse)
{
	m_name = name;
	m_URL = URL;
	m_updateSecond = updateSecond;
	m_checkParse = checkParse;

	temporaryName = m_name; // для дебагинга

	if (m_checkParse)
		classTimer->start(m_updateSecond.toInt()); // Каждые три секунды
	else
		classTimer->stop();

	QString temporary = m_URL; // обычный с сортировкой по дате

	temporary = temporary.remove("https://www.avito.ru");

	int count = 0;

	for (auto& val : temporary)
	{
		if (val == '/' || val == '?') count++;

		if (count == 3 && (val == '?' || val == '/'))
		{
			subUrlString += '/';
			break;
		}

		subUrlString += val;
	}

	qDebug() << '\n' << m_URL << subUrlString << "\n";
}


void uniqueParseObject::classTimerIsDone()
{
	generalParseFunc();
}


QList<QString> uniqueParseObject::getRefMassive()
{
	return referenceList;
}


void uniqueParseObject::setRefMassive()
{
	QFile file(m_name + "_RefMassive.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Error in uniqueParseObject::setRefMassive(not find file or other):" << file.error();
	}

	QString line;

	QTextStream in(&file);

	while (!in.atEnd())
	{
		line = in.readLine();
		referenceList.push_back(line);
	}

	file.close();

	qDebug() << "Start in " << QDateTime::currentDateTime().toString() << "(" + m_name + ")" + " with count of reference: " + QString::number(referenceList.length());
}


QTimer* uniqueParseObject::getTimer()
{
	return classTimer;
}
