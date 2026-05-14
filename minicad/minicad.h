#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_minicad.h"

class minicad : public QMainWindow
{
    Q_OBJECT

public:
    minicad(QWidget *parent = nullptr);
    ~minicad();

private:
    Ui::minicadClass ui;
};

