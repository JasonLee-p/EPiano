#pragma once
#include <Windows.h>
#include <iostream>

#include "NotePlayer.h"
#include "packages/RtMidi.h"


class MidiDeviceHandler {
	/* 用于处理midi设备，获取midi信号，发送midi信号 */

public:
	std::vector<std::string> midiInDevices;			// 所有可用的midi输入设备的名称
	std::vector<std::string> midiOutDevices;		// 所有可用的midi输出设备的名称
	std::vector<unsigned int> midiInPorts;			// 所有可用的midi输入设备的端口
	std::vector<unsigned int> midiOutPorts;			// 所有可用的midi输出设备的端口
	unsigned int midiInPort = 0;					// 当前使用的midi输入设备的端口
	unsigned int midiOutPort = 0;					// 当前使用的midi输出设备的端口

	RtMidiIn* midiin = nullptr;						// midi输入设备
	RtMidiOut* midiout = nullptr;					// midi输出设备

	MidiDeviceHandler();
	~MidiDeviceHandler();

	void initMidiPlayer();							// 初始化midi播放器
	void getMidiDevices();							// 获取midi设备信息
	void openMidiInPort(unsigned int port);			// 打开midi输入设备端口
	void openMidiOutPort(unsigned int port);		// 打开midi输出设备端口
	void closeMidiInPort();							// 关闭midi输入设备端口
	void closeMidiOutPort();						// 关闭midi输出设备端口

													// midi回调函数
	static void midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData);
													// 向midi输出设备发送消息
	void sendMidiOutMessage(unsigned char status, unsigned char data1, unsigned char data2);

};
