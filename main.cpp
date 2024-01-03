#include "loadConfig.h"
#include "MainWin.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    LoadConfig config;
    SynthNote::initAllSynthWaves();
    Key::synth = new Synth();
    QApplication a(argc, argv);
    LoadConfig config;
    MainWin w;
    Music::Chord::readChordAttr(w.statusBar());
    w.show();
    return a.exec();
}
