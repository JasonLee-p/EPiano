#pragma once

#include "note.h"
#include "midiDeviceHandler.h"

class Key {
public:
	static std::array<Key*, 88> keys;						// 88����
	static MidiDeviceHandler* midiDeviceHandler;        	// midi�豸������
	
	bool isWhite;											// �Ƿ�Ϊ�׼�
	int index = -1;											// �ںڼ���׼��е�����
	bool mousePressed = false;								// ����갴��
	int keyNumber = -1;										// ��C4Ϊ60�ż�
	int paintPosX, paintPosY = 0;							// ����ʱ������
	Note note;
	
	Key(int keyNumber);
	Key() = default;
	~Key();

	void play();											// ��������
	void stop();											// ֹͣ����

private:
	bool pressed = false;									// ���ⲿ�豸���£���play()������ʱ���޸ģ�
};