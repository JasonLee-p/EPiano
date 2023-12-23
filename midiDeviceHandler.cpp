#include "MidiDeviceHandler.h"


MidiDeviceHandler::MidiDeviceHandler() {
    initMidiPlayer();
    getMidiDevices();
    getAudioDevices();
}

MidiDeviceHandler::~MidiDeviceHandler() {
    closeMidiPort();
    closeAudioPort();

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
		midiout = new RtMidiOut();
	}
    catch (RtMidiError& error) {
		std::cerr << "Error initializing MIDI player: " << error.what() << std::endl;
	}
}

void MidiDeviceHandler::getMidiDevices() {
    // 使用Windows API获取可用的MIDI设备信息
    MIDIINCAPS midiInCaps;
    UINT numDevices = midiInGetNumDevs();
    for (UINT i = 0; i < numDevices; ++i) {
        if (midiInGetDevCaps(i, &midiInCaps, sizeof(MIDIINCAPS)) == MMSYSERR_NOERROR) {
            midiDevices.push_back(midiInCaps.szPname);
            midiPorts.push_back(i);
        }
    }
}

void MidiDeviceHandler::getAudioDevices() {
    // 使用Windows API获取可用的音频设备信息
    WAVEOUTCAPS waveOutCaps;
    UINT numDevices = waveOutGetNumDevs();

    for (UINT i = 0; i < numDevices; ++i) {
        if (waveOutGetDevCaps(i, &waveOutCaps, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR) {
            audioDevices.push_back(waveOutCaps.szPname);
            audioPorts.push_back(i);
        }
    }
}

void MidiDeviceHandler::openMidiPort(unsigned int port) {
    // 打开指定端口的MIDI设备
    closeMidiPort();  // 先关闭之前的端口

    try {
        midiin = new RtMidiIn();
        midiin->openPort(port);
        midiin->setCallback(&MidiDeviceHandler::midiCallback, this);
        midiin->ignoreTypes(false, false, false);
        midiPort = port;
    }
    catch (RtMidiError& error) {
        std::cerr << "Error opening MIDI input port: " << error.what() << std::endl;
    }
}

void MidiDeviceHandler::openAudioPort(unsigned int port) {
    // 打开指定端口的音频设备
    closeAudioPort();  // 先关闭之前的端口

    try {
        // 使用Windows API打开音频设备
        std::cout << "Opening audio port: " << port << std::endl;
        audioPort = port;
    }
    catch (std::exception& error) {
        std::cerr << "Error opening audio output port: " << error.what() << std::endl;
    }
}


void MidiDeviceHandler::closeMidiPort() {
    // 关闭MIDI设备端口
    if (midiin && midiin->isPortOpen()) {
        midiin->closePort();
        delete midiin;
        midiin = nullptr;
        midiPort = 0;
    }
}

void MidiDeviceHandler::closeAudioPort() {
    // 关闭音频设备端口
    std::cout << "Closing audio port" << std::endl;
}


void MidiDeviceHandler::sendMessage(std::vector<unsigned char>* message) {
    sendMessage(message, midiPort, midiout);
}

void MidiDeviceHandler::sendMessage(std::vector<unsigned char>* message, unsigned int port) {
    sendMessage(message, port, midiout);
}

void MidiDeviceHandler::sendMessage(std::vector<unsigned char>* message, RtMidiOut* midiout) {
    sendMessage(message, midiPort, midiout);
}

void MidiDeviceHandler::sendMessage(std::vector<unsigned char>* message, unsigned int port, RtMidiOut* midiout) {
    // 发送MIDI消息
    if (midiout && midiout->isPortOpen()) {
        try {
            midiout->openPort(port);
            midiout->sendMessage(message);

            midiout->closePort();
        }
        catch (RtMidiError& error) {
            std::cerr << "Error sending MIDI message: " << error.what() << std::endl;
        }
    }
}

void MidiDeviceHandler::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    // MIDI回调函数的实现
    std::cout << "MIDI Callback - Delta Time: " << deltatime << ", Message: ";
    for (const auto& byte : *message) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

void MidiDeviceHandler::noteOn(Note& note, int velocity) {
	// 按下音符
	std::vector<unsigned char> message;
	message.push_back(0x90);
	message.push_back(note.midiNumber);
	message.push_back(velocity);
	sendMessage(&message);
    // 播放音符
    
}

void MidiDeviceHandler::noteOff(Note& note, int velocity) {
	// 松开音符
	std::vector<unsigned char> message;
	message.push_back(0x80);
	message.push_back(note.midiNumber);
	message.push_back(velocity);
	sendMessage(&message);
}

