#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AvitoParser.h"

class AvitoParser : public QMainWindow
{
    Q_OBJECT

public:
    AvitoParser(QWidget *parent = nullptr);
    ~AvitoParser();

private:
    Ui::AvitoParserClass ui;
};
