
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

class AvitoParser : public QMainWindow
{
    Q_OBJECT

public:

    AvitoParser(QWidget* parent = nullptr);
    ~AvitoParser();



   // VisualForMilanRF(QWidget* parent = nullptr);
   // ~VisualForMilanRF();

    void addItemInList();
    void deleteItemInList();
    void adressFinder();
    void exportXml();
    void recursionXmlWriter(QTreeWidgetItem* some, QXmlStreamWriter& someXmlWriter);
    void importXml();
    void loopXmlReader(QTreeWidgetItem* some, QXmlStreamReader& xmlReader);
    void refresh();
    bool connectDB();
    void recursionDbSqlReader(QTreeWidgetItem* some);
    void browse();
    void recursionXlsWriter(QTreeWidgetItem* some);
    void report();
    void startingImportXml();
    void showRangeTable();
    void sortTable();

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
};
