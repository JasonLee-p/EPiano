#pragma once

#include "note.h"

class Key {
public:
	static std::array<Key, 88> keys; // 88个键
	bool isWhite;                    // 是否为白键
	bool mousePressed = false;       // 被鼠标按下
	bool pressed = false;            // 被外部设备按下
	int keyNumber = -1;              // 以C4为60号键
	int paintPosX, paintPosY = 0;    // 绘制时的坐标
	Note note;
	
	Key(int keyNumber);
	Key() = default;
	~Key();

	void play();                     // 播放音符
};