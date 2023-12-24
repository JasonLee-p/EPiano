#ifndef KEY_H
#define KEY_H

#include "midiDeviceHandler.h"
#include "NotePlayer.h"
#include "music.h"

#define NMOD keyNumber % 12
#define note_IS_WHITE_KEY (NMOD == 0 || NMOD == 2 || NMOD == 4 || NMOD == 5 || NMOD == 7 || NMOD == 9 || NMOD == 11)




class Key {
public:
	static MidiDeviceHandler* midiDeviceHandler;        	// midi设备处理器
	static Synth* synth;									// 音符播放器
	static unsigned int DEFAULT_VELOCITY;					// 默认音量
	
	bool isWhite;											// 是否为白键
	int index = -1;											// 在黑键或白键中的索引
	bool mousePressed = false;								// 被鼠标按下
	int keyNumber = -1;										// 以C4为60号键
	int paintPosX, paintPosY = 0;							// 绘制时的坐标
	Music::Note note;
	
	Key(int keyNumber) : keyNumber(keyNumber) {
		note = Music::Note(keyNumber);
		isWhite = note_IS_WHITE_KEY;
	}
	Key() = default;

	~Key() {};

	inline void play() {											// 播放音符
		std::cout << "[INFO] play: " << keyNumber << std::endl;
		pressed = true;
		synth->noteOn(this->note, DEFAULT_VELOCITY);
		// 向外输出midi信号
		midiDeviceHandler->sendMidiOutMessage(NOTE_ON, keyNumber, DEFAULT_VELOCITY);
	}

	inline void stop() {											// 停止播放
		std::cout << "[INFO] stop: " << keyNumber << std::endl;
		synth->noteOff(this->note, DEFAULT_VELOCITY);
		// 向外输出midi信号
		midiDeviceHandler->sendMidiOutMessage(NOTE_OFF, keyNumber, DEFAULT_VELOCITY);
		pressed = false;
	}

private:
	bool pressed = false;									// 被外部设备按下（当play()被调用时，修改）
};

#endif // !KEY_H