#include"key.h"

// Key��ľ�̬��Ա������ʼ��
MidiDeviceHandler* Key::midiDeviceHandler = new MidiDeviceHandler();
Synth* Key::synth = nullptr;
unsigned int Key::DEFAULT_VELOCITY = 100;