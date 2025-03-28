#include "uniqueParseObject.h"

uniqueParseObject::uniqueParseObject(QObject* parent)
	: QObject(parent), classTimer(new QTimer())
{
	connect(classTimer, &QTimer::timeout, this, &uniqueParseObject::classTimerIsDone);
}

void uniqueParseObject::generalParseFunc()
{
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
		nam.setAutoDeleteReplies(true);

		//int page = 1;// при многостраничном поиске
		//for (int val = 1; val <= 4; val++) // при многостраничном поиске
			//QString temporaryUrl = urlString;// при многостраничном поиске
			//temporaryUrl.insert((urlString.indexOf("cd=1") + 4), "&p=" + QString::number(page));// при многостраничном поиске

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

		//page++;// при многостраничном поиске

		//if (true)
		if (countOfReference < referenceList.length())
		{
			qDebug() << "\n" << QDateTime::currentDateTime().toString() << "(" + m_name + ")" + " count of reference: " + QString::number(referenceList.length());
			countOfReference = int(referenceList.length());
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

	QTextStream in(&file);

	in << data << Qt::endl;

	in.seek(0);

	while (!in.atEnd())
	{
		QString line = in.readLine();

		if (line.indexOf(subUrlString) != -1)
		{
			int index = line.indexOf(subUrlString);

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

				if (firstAccumulateReferenceValue == 0)
				{
					emit messageReceived(referenceList.last());
				}
			}
		}
	}

	file.close();
}



void uniqueParseObject::setParam(QString name, QString URL, QString updateSecond, bool checkParse)
{
	m_name = name;
	m_URL = URL;
	m_updateSecond = updateSecond;
	m_checkParse = checkParse;

	if (m_checkParse)
		classTimer->start(m_updateSecond.toInt()); // Каждые три секунды
	else
		classTimer->stop();

	QString temporary = m_URL; // обычный с сортировкой по дате

	//qDebug() << m_URL;

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

	qDebug() << m_URL << subUrlString << "\n";
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

	qDebug() << m_name << " count = " << referenceList.length();
}
