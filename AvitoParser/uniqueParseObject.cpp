#include "uniqueParseObject.h"

uniqueParseObject::uniqueParseObject(QObject *parent)
	: QObject(parent), classTimer(new QTimer())
{
	connect(classTimer, &QTimer::timeout, this, &uniqueParseObject::classTimerIsDone);
}

uniqueParseObject::~uniqueParseObject()
{}

void uniqueParseObject::generalParseFunc()
{
	if (firstAccumulateReferenceValue > 0)
		firstAccumulateReferenceValue--;

	//QList<QNetworkReply::RawHeaderPair> testListSecond;
	//QList<QByteArray> testList;

	QNetworkProxyFactory::setUseSystemConfiguration(true);
	QNetworkAccessManager nam;
	nam.setAutoDeleteReplies(true);

	QString urlString = "https://www.avito.ru/nizhnevartovsk/igry_pristavki_i_programmy/igry_dlya_pristavok-ASgBAgICAUSSAsYJ?cd=1&q=%D0%B8%D0%B3%D1%80%D1%8B+%D0%BD%D0%B0+playstation+5&s=104"; // ������� � ����������� �� ����

	//int page = 1;// ��� ��������������� ������
	//for (int val = 1; val <= 4; val++) // ��� ��������������� ������
		//QString temporaryUrl = urlString;// ��� ��������������� ������
		//temporaryUrl.insert((urlString.indexOf("cd=1") + 4), "&p=" + QString::number(page));// ��� ��������������� ������

	QEventLoop loop;

	QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

	QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl(urlString))));

	loop.exec();

	//� ������ ������ �� ���������
	//testList = reply->rawHeaderList(); // ������ ����������
	//testListSecond = reply->rawHeaderPairs(); // ������ ��� �������������� ����������

	//QFile file("BUFFER" + QString::number(page) + ".txt"); // ��� ��������������� ������ �������� ���������� ������
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

				if (firstAccumulateReferenceValue == 0)
				{
					emit messageReceived(temporary);
				}
			}
		}
	}

	file.close();

	//page++;// ��� ��������������� ������

	/*
	for (auto& val : referenceList)
	{
		qDebug() << val;
	}
	*/

	qDebug() << "\n" << "Count of reference: " + QString::number(referenceList.length());
}

void uniqueParseObject::setParam(QString name, QString URL, QString updateSecond, bool checkParse)
{
	m_name = name;
	m_URL = URL;
	m_updateSecond = updateSecond;
	m_checkParse = checkParse;

	if (m_checkParse)
		classTimer->start(m_updateSecond.toInt()); // ������ ��� �������
	else
		classTimer->stop();
}


void uniqueParseObject::classTimerIsDone()
{
	generalParseFunc();
	//qDebug() << m_name + " is Done!";
}

