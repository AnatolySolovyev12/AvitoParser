#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QFile>
#include <QTimer>
#include <QDateTime>

class uniqueParseObject : public QObject
{
	Q_OBJECT

public:
	uniqueParseObject(QObject* parent = nullptr);

	~uniqueParseObject();


	void setParam(QString name, QString URL, QString updateSecond, bool checkParse);
	void generalParseFunc();

signals:
	void messageReceived(const QString&);

private:
	void classTimerIsDone();
	
private:

	QString m_name;
	QString m_URL;
	QString m_updateSecond;
	bool m_checkParse = false;

	QTimer* classTimer = nullptr;

	QList<QString>referenceList;

	int firstAccumulateReferenceValue = 6;
	int countOfReference;

	QString LastTemporaryMessege;

	QString subUrlString;
};
