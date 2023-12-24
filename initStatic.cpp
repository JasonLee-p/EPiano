/*
	静态成员变量
*/

#include "keysettingbutton.h"


// Note类的静态成员变量初始化
std::map<int, std::string> Music::Note::PITCH_CLASS2NAME = {
	   {0, std::string("C")},
	   {1, std::string("C#")},
	   {2, std::string("D")},
	   {3, std::string("D#")},
	   {4, std::string("E")},
	   {5, std::string("F")},
	   {6, std::string("F#")},
	   {7, std::string("G")},
	   {8, std::string("G#")},
	   {9, std::string("A")},
	   {10, std::string("A#")},
	   {11, std::string("B")}
};

// KeySettingButton类的静态成员变量初始化
KeySettingButton* KeySettingButton::currentButton = nullptr;