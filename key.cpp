#include"key.h"

// Key类的静态成员变量初始化
MidiDeviceHandler* Key::midiDeviceHandler = new MidiDeviceHandler();
Synth* Key::synth = nullptr;
unsigned int Key::DEFAULT_VELOCITY = 100;