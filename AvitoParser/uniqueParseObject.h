#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkProxyFactory>
#include <QFile>

class uniqueParseObject  : public QObject
{
	Q_OBJECT

public:
	uniqueParseObject(QObject *parent);
	~uniqueParseObject();

	void generalParseFunc();
};
