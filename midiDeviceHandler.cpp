#include "MidiDeviceHandler.h"


MidiDeviceHandler* MidiDeviceHandler::currentHandler = nullptr;

MidiDeviceHandler::MidiDeviceHandler() {
	currentHandler = this;
    initMidiPlayer();
    getMidiDevices();
	openMidiInPort(0);
	// openMidiOutPort(0);
}

MidiDeviceHandler::~MidiDeviceHandler() {
    closeMidiInPort();
    closeMidiOutPort();

    if (midiin) {
        delete midiin;
    }
    if (midiout) {
        delete midiout;
    }
}

void MidiDeviceHandler::initMidiPlayer() {
    // 初始化MIDI播放器（rtmidi库）
    try {
        midiin = new RtMidiIn();
		midiout = new RtMidiOut();
	}
    catch (RtMidiError& error) {
		std::cerr << "[ERROR] Failed to initialize MIDI player: " << error.what() << std::endl;
	}
}

void MidiDeviceHandler::getMidiDevices() {
    // 输入设备
    unsigned int nPorts = midiin->getPortCount();
    std::cout << "[INFO] MIDI input ports:" << std::endl;
    std::string portName;
    for (unsigned int i = 0; i < nPorts; i++) {
        try {
            portName = midiin->getPortName(i);
            midiInDevices.push_back(portName);
            midiInPorts.push_back(i);
        }
        catch (RtMidiError& error) {
            error.printMessage();
        }
        std::cout << "[INFO] -Input Port #" << i + 1 << ": " << portName << '\n';
    }
    // 输出设备
    nPorts = midiout->getPortCount();
    std::cout << "[INFO] MIDI output ports:" << std::endl;
    for (unsigned int i = 0; i < nPorts; i++) {
        try {
			portName = midiout->getPortName(i);
			midiOutDevices.push_back(portName);
			midiOutPorts.push_back(i);
		}
        catch (RtMidiError& error) {
			error.printMessage();
		}
		std::cout << "[INFO] -Output Port #" << i + 1 << ": " << portName << '\n';
	}
}


void MidiDeviceHandler::openMidiInPort(unsigned int port) {
	// 打开指定端口的MIDI输入设备
	closeMidiInPort();  // 先关闭之前的端口
    try {
		// 使用rtmidi库打开MIDI输入设备
		std::cout << "[INFO] Opening MIDI input port: " << port << std::endl;
		midiin->openPort(port);
		midiin->setCallback(&MidiDeviceHandler::midiCallback, this);
		midiin->ignoreTypes(false, false, false);
		midiInPort = port;
	}
    catch (RtMidiError& error) {
		std::cerr << "[ERROR] Failed to open MIDI input port: " << error.what() << std::endl;
	}
}

void MidiDeviceHandler::openMidiOutPort(unsigned int port) {
	// 打开指定端口的MIDI输出设备
	closeMidiOutPort();  // 先关闭之前的端口

    try {
		// 使用rtmidi库打开MIDI输出设备
		std::cout << "[INFO] Opening MIDI output port: " << port << std::endl;
		midiout->openPort(port);
		midiOutPort = port;
	}
    catch (RtMidiError& error) {
		std::cerr << "[ERROR] Failed to open MIDI output port: " << error.what() << std::endl;
	}
}


void MidiDeviceHandler::closeMidiInPort() {
	// 关闭MIDI输入设备端口
    if (midiin) {
		std::cout << "[INFO] Closing MIDI input port" << midiInPort << std::endl;
		midiin->closePort();
	}
}

void MidiDeviceHandler::closeMidiOutPort() {
	// 关闭MIDI输出设备端口
    if (midiout) {
		std::cout << "[INFO] Closing MIDI output port" << midiOutPort << std::endl;
		midiout->closePort();
	}
}

void MidiDeviceHandler::sendMidiOutMessage(unsigned char status, unsigned char data1, unsigned char data2) {
	// 向MIDI输出设备发送消息
	std::vector<unsigned char> message;
	message.push_back(status);
	message.push_back(data1);
	message.push_back(data2);
	midiout->sendMessage(&message);
}

void MidiDeviceHandler::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    // 收到MIDI消息时的回调函数，播放MIDI消息（byte0为起音或停音，byte1为音高，byte2为力度）
	unsigned int nBytes = message->size();
	if (nBytes == 3) {
		unsigned char status = message->at(0);
		unsigned char data1 = message->at(1);
		unsigned char data2 = message->at(2);
		int data1Int = (int)data1;
		int data2Int = (int)data2;
		// 判断是否为起音或停音
		if (status == NOTE_ON) {
			// 发送给PianoGLWin处理
			emit currentHandler->sendNoteOnMessage(data1Int, data2Int);
		}
		else if (status == NOTE_OFF) {
			// 发送给PianoGLWin处理
			emit currentHandler->sendNoteOffMessage(data1Int, data2Int);
		}
	}
}
