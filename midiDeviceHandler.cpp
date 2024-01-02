#include "MidiDeviceHandler.h"


MidiDeviceHandler::MidiDeviceHandler() {
    initMidiPlayer();
    getMidiDevices();

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
    // ��ʼ��MIDI��������rtmidi�⣩
    try {
        midiin = new RtMidiIn();
		midiout = new RtMidiOut();
	}
    catch (RtMidiError& error) {
		std::cerr << "[ERROR] Failed to initialize MIDI player: " << error.what() << std::endl;
	}
}

void MidiDeviceHandler::getMidiDevices() {
    // �����豸
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
    // ����豸
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
	// ��ָ���˿ڵ�MIDI�����豸
	closeMidiInPort();  // �ȹر�֮ǰ�Ķ˿�

    try {
		// ʹ��rtmidi���MIDI�����豸
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
	// ��ָ���˿ڵ�MIDI����豸
	closeMidiOutPort();  // �ȹر�֮ǰ�Ķ˿�

    try {
		// ʹ��rtmidi���MIDI����豸
		std::cout << "[INFO] Opening MIDI output port: " << port << std::endl;
		midiout->openPort(port);
		midiOutPort = port;
	}
    catch (RtMidiError& error) {
		std::cerr << "[ERROR] Failed to open MIDI output port: " << error.what() << std::endl;
	}
}


void MidiDeviceHandler::closeMidiInPort() {
	// �ر�MIDI�����豸�˿�
	std::cout << "[INFO] Closing MIDI input port" << std::endl;
    if (midiin) {
		midiin->closePort();
	}
}

void MidiDeviceHandler::closeMidiOutPort() {
	// �ر�MIDI����豸�˿�
	std::cout << "[INFO] Closing MIDI output port" << std::endl;
    if (midiout) {
		midiout->closePort();
	}
}

void MidiDeviceHandler::sendMidiOutMessage(unsigned char status, unsigned char data1, unsigned char data2) {
	// ��MIDI����豸������Ϣ
	std::vector<unsigned char> message;
	message.push_back(status);
	message.push_back(data1);
	message.push_back(data2);
	midiout->sendMessage(&message);
}

void MidiDeviceHandler::midiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
    // MIDI�ص�������ʵ��
    std::cout << "[INFO] MIDI message: " << std::hex;
    for (const auto& byte : *message) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}
