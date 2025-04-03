#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AvitoParser.h"
#include <QFileDialog>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>
#include <QMouseEvent>
#include <uniqueParseObject.h>
#include "TelegramJacket.h"

class AvitoParser : public QMainWindow
{
    Q_OBJECT

public:

    AvitoParser(QWidget* parent = nullptr);
    ~AvitoParser();

    void addItemInList();
    void deleteItemInList();
    void exportXml();
    void recursionXmlWriter(QTreeWidgetItem * some, QXmlStreamWriter& someXmlWriter);
    void importXml();
    void loopXmlReader(QXmlStreamReader& xmlReader);
    void startingImportXml();
    void mousePressEvent(QMouseEvent* event);
    void initializationPoolFunc();

    void setData();
    void closeEditor(QTreeWidgetItem* any);
    void otherItemWasChecked(QTreeWidgetItem* any);

    void saveRefMassive();

    void generalFuncForTimer();

private:
    Ui::AvitoParserClass ui;

    bool offChanger = false;
    bool sortBool = true;

    int middleColumn = 0;
    int countRow = 1;

    QTreeWidgetItem* middleItem = nullptr;
    QStatusBar* sBar;

    QString line;

    QList<QSharedPointer<uniqueParseObject>>poolParse;
    QTimer * timer = nullptr;
    QTimer* timerSemafor = nullptr;
    TelegramJacket* tgObject = nullptr;
};
