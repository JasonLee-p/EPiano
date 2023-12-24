#pragma once

#include <QtWidgets/QMainWindow>
#include <QtGui/QOpenGLFunctions>
#include <QMutexLocker>
#include "loadConfig.h"
#include "music.h"
#include "ui_MainWin.h"
#include "ui_SettingsWin.h"

#define CHECK_UPDATE if (updating) while (updating) {}; updating = true;
#define END_UPDATE updating = false;

class SettingsWin : public QDialog
{
    Q_OBJECT
    friend class MainWin;

public:
    SettingsWin(QWidget* parent = nullptr);
    ~SettingsWin();

    QMutex mutex;

private:
    Ui::SettingsWinClass ui;
};




class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = nullptr);
    ~MainWin();

    SettingsWin *settingsDialog;
    bool updating = false;
    QMutex mutex;
    void updatePressedKeysMessage();

private:
    Ui::MainWinClass ui;

    void connectActions();
    void initInputConfig();

    void on_oiSetting_triggered();
    void on_keyActionSetting_triggered();
    void on_otherSetting_triggered();
    void on_aboutAuthor_triggered();
    void on_aboutQt_triggered();
};
