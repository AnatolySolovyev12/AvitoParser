
#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AvitoParser.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamAttribute>
#include <QtSql/QtSql>
#include <QTableWidgetItem>

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








private slots:
    void setData();
    void closeEditor(QTreeWidgetItem* any);
    void otherItemWasChecked(QTreeWidgetItem* any);

private:
    Ui::AvitoParserClass ui;

    QTreeWidgetItem* middleItem = nullptr;
    int middleColumn;
    bool offChanger = false;
    QString line;
    QSqlDatabase mw_db; // экземпляр подключения к БД

    QTableWidgetItem* item = nullptr;
    int countRow = 1;
    QStatusBar* sBar;

    bool sortBool = true;

    QList<QSharedPointer<uniqueParseObject>>poolParse;

    QTimer * timer = nullptr;

    TelegramJacket* tgObject = nullptr;

};
