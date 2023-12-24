#include "loadConfig.h"

// 键盘映射
std::unordered_map<int, int> LoadConfig::KEYMAP;
std::map<std::string, int> LoadConfig::KEYNAME2QTKEY = {
	{"keyZ", Qt::Key_Z},	{"keyX", Qt::Key_X},	{"keyC", Qt::Key_C},	{"keyV", Qt::Key_V},
	{"keyB", Qt::Key_B},	{"keyN", Qt::Key_N},	{"keyM", Qt::Key_M},	{"keyA", Qt::Key_A},
	{"keyS", Qt::Key_S},	{"keyD", Qt::Key_D},	{"keyF", Qt::Key_F},	{"keyG", Qt::Key_G},
	{"keyH", Qt::Key_H},	{"keyJ", Qt::Key_J},	{"keyQ", Qt::Key_Q},	{"keyW", Qt::Key_W},
	{"keyE", Qt::Key_E},	{"keyR", Qt::Key_R},	{"keyT", Qt::Key_T},	{"keyY", Qt::Key_Y},
	{"keyU", Qt::Key_U},	{"keyI", Qt::Key_I},	{"keyO", Qt::Key_O},	{"keyP", Qt::Key_P},
	{"keyL", Qt::Key_L},	{"keyK", Qt::Key_K},	{"key1", Qt::Key_1},	{"key2", Qt::Key_2},
	{"key3", Qt::Key_3},	{"key4", Qt::Key_4},	{"key5", Qt::Key_5},	{"key6", Qt::Key_6},
	{"key7", Qt::Key_7},	{"key8", Qt::Key_8},	{"key9", Qt::Key_9},	{"key0", Qt::Key_0},
	{"keyMinus", Qt::Key_Minus},					{"keyEqual", Qt::Key_Equal},
	{"keyBackslash", Qt::Key_Backslash},			{"keyBracketLeft", Qt::Key_BracketLeft},
	{"keyBracketRight", Qt::Key_BracketRight},		{"keySemicolon", Qt::Key_Semicolon},
	{"keyApostrophe", Qt::Key_Apostrophe},			{"keyComma", Qt::Key_Comma},
	{"keyPeriod", Qt::Key_Period},					{"keySlash", Qt::Key_Slash}
};

// 音频参数
unsigned int												LoadConfig::CHANNELS;
unsigned int												LoadConfig::SAMPLE_RATE;
unsigned int												LoadConfig::BUFFER_SIZE;

// 合成器参数
std::array<unsigned int, 4>									LoadConfig::ENVELOPE_PARAMETERS;
std::array<unsigned int, 13>								LoadConfig::HARMONIC_INTENSITIES;

// 和弦参数
std::unordered_map<std::string, std::array<bool, 12>>		LoadConfig::CHORDNAME2PITCHCLASSES;
std::unordered_map<std::array<bool, 12>, std::string>		LoadConfig::PITCHCLASSES2CHORDNAME;
std::unordered_map<std::string, Music::ChordColorVector>	LoadConfig::CHORDNAME2COLORVECTOR;
std::unordered_map<int, std::string>						LoadConfig::INTERVAL2NAME;