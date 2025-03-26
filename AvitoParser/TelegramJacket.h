#pragma once

#include <QObject>

class TelegramJacket  : public QObject
{
	Q_OBJECT

public:
	TelegramJacket(QObject *parent);
	~TelegramJacket();
};
