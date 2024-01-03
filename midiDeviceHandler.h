#pragma once
#include <Windows.h>
#include <iostream>
#include <QObject>
#include "NotePlayer.h"
#include "packages/RtMidi.h"

#define NOTE_ON 0x90
#define NOTE_OFF 0x80


class MidiDeviceHandler : public QObject {
	Q_OBJECT
	/* ���ڴ���midi�豸����ȡmidi�źţ�����midi�ź� */
signals:
	void sendNoteOnMessage(unsigned int keyNumber, unsigned int velocity);		// ����noteOn��Ϣ
	void sendNoteOffMessage(unsigned int keyNumber, unsigned int velocity);		// ����noteOff��Ϣ

public:
	static MidiDeviceHandler* currentHandler;		// ��ǰ��midi�豸������

	std::vector<std::string> midiInDevices;			// ���п��õ�midi�����豸������
	std::vector<std::string> midiOutDevices;		// ���п��õ�midi����豸������
	std::vector<unsigned int> midiInPorts;			// ���п��õ�midi�����豸�Ķ˿�
	std::vector<unsigned int> midiOutPorts;			// ���п��õ�midi����豸�Ķ˿�
	unsigned int midiInPort = 0;					// ��ǰʹ�õ�midi�����豸�Ķ˿�
	unsigned int midiOutPort = 0;					// ��ǰʹ�õ�midi����豸�Ķ˿�

	RtMidiIn* midiin = nullptr;						// midi�����豸
	RtMidiOut* midiout = nullptr;					// midi����豸

	MidiDeviceHandler();
	~MidiDeviceHandler();

	void initMidiPlayer();							// ��ʼ��midi������
	void getMidiDevices();							// ��ȡmidi�豸��Ϣ
	void openMidiInPort(unsigned int port);			// ��midi�����豸�˿�
	void openMidiOutPort(unsigned int port);		// ��midi����豸�˿�
	void closeMidiInPort();							// �ر�midi�����豸�˿�
	void closeMidiOutPort();						// �ر�midi����豸�˿�

													// midi�ص�����
	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
													// ��midi����豸������Ϣ
	void sendMidiOutMessage(unsigned char status, unsigned char data1, unsigned char data2);

};
