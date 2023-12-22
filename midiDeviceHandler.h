#pragma once
#include <Windows.h>
#include <iostream>

#include "packages/RtMidi.h"


class MidiDeviceHandler {
	/* ���ڴ���midi�豸����ʼ��midi����������ȡmidi�豸��Ϣ�� */

public:
	std::vector<std::wstring> midiDevices;			// ���п��õ�midi�豸������
	std::vector<std::wstring> audioDevices;			// ���п��õ���������Ƶ�豸������
	std::vector<unsigned int> midiPorts;			// ���п��õ�midi�豸�Ķ˿�
	std::vector<unsigned int> audioPorts;			// ���п��õ���������Ƶ�豸�Ķ˿�
	unsigned int midiPort = 0;						// ��ǰʹ�õ�midi�豸�Ķ˿�
	unsigned int audioPort = 0;						// ��ǰʹ�õ���������Ƶ�豸�Ķ˿�

	RtMidiIn* midiin = nullptr;						// midi�����豸
	RtMidiOut* midiout = nullptr;					// midi����豸

	MidiDeviceHandler();
	~MidiDeviceHandler();

	void initMidiPlayer();							// ��ʼ��midi������
	void getMidiDevices();							// ��ȡmidi�豸��Ϣ
	void getAudioDevices();							// ��ȡ��������Ƶ�豸��Ϣ
	void openMidiPort(unsigned int port);			// ��midi�豸�˿�
	void openAudioPort(unsigned int port);			// ����������Ƶ�豸�˿�
	void closeMidiPort();							// �ر�midi�豸�˿�
	void closeAudioPort();							// �ر���������Ƶ�豸�˿�

	void sendMessage(std::vector<unsigned char>* message);												// ����midi��Ϣ
	void sendMessage(std::vector<unsigned char>* message, unsigned int port);							// ����midi��Ϣ
	void sendMessage(std::vector<unsigned char>* message, RtMidiOut* midiout);							// ����midi��Ϣ
	void sendMessage(std::vector<unsigned char>* message, unsigned int port, RtMidiOut* midiout);		// ����midi��Ϣ

	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);	// midi�ص�����

	void noteOn(int noteNumber, int velocity);		// ��������
	void noteOff(int noteNumber, int velocity);		// �ɿ�����
};
