#include"key.h"


#define NMOD keyNumber % 12
#define note_IS_WHITE_KEY (NMOD == 0 || NMOD == 2 || NMOD == 4 || NMOD == 5 || NMOD == 7 || NMOD == 9 || NMOD == 11)


MidiDeviceHandler* Key::midiDeviceHandler = nullptr;

Key::Key(int keyNumber) : keyNumber(keyNumber) {
	note = Note(keyNumber);
	isWhite = note_IS_WHITE_KEY;
}

Key::~Key() {}

void Key::play() {
	pressed = true;
	midiDeviceHandler->noteOn(note, 127);
}

void Key::stop() {
	midiDeviceHandler->noteOff(note, 127);
	pressed = false;
}