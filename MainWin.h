#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_MainWin.h"

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = nullptr);
    ~MainWin();

private:
    Ui::MainWinClass ui;
};
