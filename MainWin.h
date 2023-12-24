#pragma once

#include <QtWidgets/QMainWindow>
#include <QtGui/QOpenGLFunctions>
#include <QMutexLocker>
#include "chord.h"
#include "ui_MainWin.h"

#define CHECK_UPDATE if (updating) while (updating) {}; updating = true;
#define END_UPDATE updating = false;

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = nullptr);
    ~MainWin();

    bool updating = false;
    QMutex mutex;
    void updatePressedKeysMessage();

private:
    Ui::MainWinClass ui;
};
