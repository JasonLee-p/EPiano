#pragma once

#include "note.h"
#include "midiDeviceHandler.h"

class Key {
public:
	static std::array<Key*, 88> keys;						// 88个键
	static MidiDeviceHandler* midiDeviceHandler;        	// midi设备处理器
	
	bool isWhite;											// 是否为白键
	int index = -1;											// 在黑键或白键中的索引
	bool mousePressed = false;								// 被鼠标按下
	int keyNumber = -1;										// 以C4为60号键
	int paintPosX, paintPosY = 0;							// 绘制时的坐标
	Note note;
	
	Key(int keyNumber);
	Key() = default;
	~Key();

	void play();											// 播放音符
	void stop();											// 停止播放

private:
	bool pressed = false;									// 被外部设备按下（当play()被调用时，修改）
};