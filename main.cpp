#include "MainWin.h"
#include "PianoGLWin.h"
#include "MidiDeviceHandler.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    MidiDeviceHandler midiDeviceHandler;
    Key::midiDeviceHandler = &midiDeviceHandler;
    w.show();
    return a.exec();
}
