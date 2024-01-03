#ifndef KEY_H
#define KEY_H

#include "midiDeviceHandler.h"
#include "NotePlayer.h"
#include "music.h"

#define NMOD keyNumber % 12
#define note_IS_WHITE_KEY (NMOD == 0 || NMOD == 2 || NMOD == 4 || NMOD == 5 || NMOD == 7 || NMOD == 9 || NMOD == 11)




class Key {
public:
	static MidiDeviceHandler* midiDeviceHandler;        	// midi�豸������
	static Synth* synth;									// ����������
	static unsigned int DEFAULT_VELOCITY;					// Ĭ������
	
	bool isWhite;											// �Ƿ�Ϊ�׼�
	int index = -1;											// �ںڼ���׼��е�����
	bool mousePressed = false;								// ����갴��
	int keyNumber = -1;										// ��C4Ϊ60�ż�
	int paintPosX, paintPosY = 0;							// ����ʱ������
	Music::Note note;
	
	Key(int keyNumber) : keyNumber(keyNumber) {
		note = Music::Note(keyNumber);
		isWhite = note_IS_WHITE_KEY;
	}
	Key() = default;

	~Key() {};

	inline void play() {											// ��������
		std::cout << "[INFO] play: " << keyNumber << std::endl;
		pressed = true;
		synth->noteOn(this->note, DEFAULT_VELOCITY);
		// �������midi�ź�
		midiDeviceHandler->sendMidiOutMessage(NOTE_ON, keyNumber, DEFAULT_VELOCITY);
	}

	inline void stop() {											// ֹͣ����
		std::cout << "[INFO] stop: " << keyNumber << std::endl;
		synth->noteOff(this->note, DEFAULT_VELOCITY);
		// �������midi�ź�
		midiDeviceHandler->sendMidiOutMessage(NOTE_OFF, keyNumber, DEFAULT_VELOCITY);
		pressed = false;
	}

private:
	bool pressed = false;									// ���ⲿ�豸���£���play()������ʱ���޸ģ�
};

#endif // !KEY_H