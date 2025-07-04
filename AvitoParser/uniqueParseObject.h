﻿#pragma once

#include<qapplication.h>
#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QFile>
#include <QTimer>
#include <QDateTime>
#include <windows.h>


class uniqueParseObject : public QObject
{
	Q_OBJECT

public:
	uniqueParseObject(QObject* parent = nullptr);

	void setParam(QString name, QString URL, QString updateSecond, bool checkParse);
	void generalParseFunc();
	void fileParseFunc(const QByteArray& data);

	QList<QString> getRefMassive();

	void setRefMassive();

	QTimer* getTimer();

	QString temporaryName; // для дебагинга

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

	int firstAccumulateReferenceValue = 5;
	int countOfReference = 0;

	QString subUrlString;

	QString potentialNewString;

	QString prefix;
	QString stampPrefix;
};
