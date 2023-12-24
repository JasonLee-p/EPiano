#include "MainWin.h"
#include "chord.h"
#include "PianoGLWin.h"
#include "MidiDeviceHandler.h"
#include <QtWidgets/QApplication>

void about() {}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    MusicELements::Chord::readChordAttr();
    MidiDeviceHandler midiDeviceHandler;
    Key::midiDeviceHandler = &midiDeviceHandler;
    w.show();
    return a.exec();
}
