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

	QNetworkProxyFactory::setUseSystemConfiguration(true);
	QNetworkAccessManager nam;
	nam.setAutoDeleteReplies(true);

	//int page = 1;// ��� ��������������� ������
	//for (int val = 1; val <= 4; val++) // ��� ��������������� ������
		//QString temporaryUrl = urlString;// ��� ��������������� ������
		//temporaryUrl.insert((urlString.indexOf("cd=1") + 4), "&p=" + QString::number(page));// ��� ��������������� ������

	QEventLoop loop;

	QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

	QSharedPointer<QNetworkReply>reply(nam.get(QNetworkRequest(QUrl(m_URL))));

	loop.exec();

    QFile file("BUFFER" + m_name + ".txt"); // ��� ��������������� ������ �������� ���������� ������

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

				if (firstAccumulateReferenceValue == 0 && (LastTemporaryMessege != temporary))
				{
					LastTemporaryMessege = temporary;
					emit messageReceived(temporary);
				}
			}
		}
	}

	file.close();

	//page++;// ��� ��������������� ������
	if (countOfReference != referenceList.length())
	{
		qDebug() << "\n" << QDateTime::currentDateTime().toString() << "(" + m_name + ")" + " count of reference: " + QString::number(referenceList.length());
		countOfReference = referenceList.length();
	}
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

	QString temporary = m_URL; // ������� � ����������� �� ����

	qDebug() << m_URL;

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

	qDebug() << subUrlString << "\n";
}


void uniqueParseObject::classTimerIsDone()
{
	generalParseFunc();
}

