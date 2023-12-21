#pragma once

#include "note.h"

class Key {
public:
	static std::array<Key, 88> keys; // 88����
	bool isWhite;                    // �Ƿ�Ϊ�׼�
	bool mousePressed = false;       // ����갴��
	bool pressed = false;            // ���ⲿ�豸����
	int keyNumber = -1;              // ��C4Ϊ60�ż�
	int paintPosX, paintPosY = 0;    // ����ʱ������
	Note note;
	
	Key(int keyNumber);
	Key() = default;
	~Key();

	void play();                     // ��������
};