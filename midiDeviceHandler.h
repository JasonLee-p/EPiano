#pragma once
#include <Windows.h>
#include <iostream>

#include "packages/RtMidi.h"


class MidiDeviceHandler {
	/* 用于处理midi设备，初始化midi播放器，获取midi设备信息等 */

public:
	std::vector<std::wstring> midiDevices;			// 所有可用的midi设备的名称
	std::vector<std::wstring> audioDevices;			// 所有可用的声卡或音频设备的名称
	std::vector<unsigned int> midiPorts;			// 所有可用的midi设备的端口
	std::vector<unsigned int> audioPorts;			// 所有可用的声卡或音频设备的端口
	unsigned int midiPort = 0;						// 当前使用的midi设备的端口
	unsigned int audioPort = 0;						// 当前使用的声卡或音频设备的端口

	RtMidiIn* midiin = nullptr;						// midi输入设备
	RtMidiOut* midiout = nullptr;					// midi输出设备

	MidiDeviceHandler();
	~MidiDeviceHandler();

	void initMidiPlayer();							// 初始化midi播放器
	void getMidiDevices();							// 获取midi设备信息
	void getAudioDevices();							// 获取声卡或音频设备信息
	void openMidiPort(unsigned int port);			// 打开midi设备端口
	void openAudioPort(unsigned int port);			// 打开声卡或音频设备端口
	void closeMidiPort();							// 关闭midi设备端口
	void closeAudioPort();							// 关闭声卡或音频设备端口

	void sendMessage(std::vector<unsigned char>* message);												// 发送midi消息
	void sendMessage(std::vector<unsigned char>* message, unsigned int port);							// 发送midi消息
	void sendMessage(std::vector<unsigned char>* message, RtMidiOut* midiout);							// 发送midi消息
	void sendMessage(std::vector<unsigned char>* message, unsigned int port, RtMidiOut* midiout);		// 发送midi消息

	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);	// midi回调函数

	void noteOn(int noteNumber, int velocity);		// 按下音符
	void noteOff(int noteNumber, int velocity);		// 松开音符
};
