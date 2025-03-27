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

//#include <QInputDialog>
//#include <QtSql/QtSql>
//#include <QTableWidgetItem>

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

    bool offChanger = false;
    bool sortBool = true;

    int middleColumn = 0;
    int countRow = 1;

    // QTableWidgetItem* item = nullptr;
    // QSqlDatabase mw_db; // экземпляр подключения к БД

    QTreeWidgetItem* middleItem = nullptr;
    QStatusBar* sBar;

    QString line;

    QList<QSharedPointer<uniqueParseObject>>poolParse;
    QTimer * timer = nullptr;
    TelegramJacket* tgObject = nullptr;
};
