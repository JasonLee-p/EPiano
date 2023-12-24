/*
	读取配置文件的类
*/
#ifndef LOADCONFIG_H
#define LOADCONFIG_H

#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <string>
#include <array>
#include <QMessageBox>
#include <QKeySequence>
#include "json.hpp"

#define NONE_NOTE 9999
constexpr auto DEFAULT_CONFIG_STRING = R"({"audioConfig":{"bufferSize":1024,"channels":2,"sampleRate":44100},"keySettingConfig":{"key0":87,"key1":9999,"key2":73,"key3":75,"key4":9999,"key5":78,"key6":80,"key7":82,"key8":9999,"key9":85,"keyA":9999,"keyApostrophe":9999,"keyB":62,"keyBracketLeft":89,"keyBracketRight":91,"keyC":59,"keyComma":67,"keyD":58,"keyE":76,"keyEqual":90,"keyF":9999,"keyG":61,"keyH":63,"keyI":84,"keyJ":9999,"keyK":66,"keyL":68,"keyM":65,"keyMinus":9999,"keyN":64,"keyO":86,"keyP":88,"keyPeriod":69,"keyQ":72,"keyR":77,"keyS":56,"keySemicolon":70,"keySlash":71,"keyT":79,"keyU":83,"keyV":60,"keyW":74,"keyX":57,"keyY":81,"keyZ":55},"synthConfig":{"envelopeParameters":[8,32,96,8],"harmonicIntensities":[127,120,110,100,90,80,70,60,50,40,30,20,10]}})";

using json = nlohmann::json;

namespace std {
	// 定义 std::array<bool, 12> 的 hash 方法，用于 unordered_map
	template <>
	struct hash<std::array<bool, 12>> {
		size_t operator()(const std::array<bool, 12>& arr) const {
			size_t hashValue = 0;
			for (bool b : arr) {
				hashValue ^= std::hash<bool>()(b);
			}
			return hashValue;
		}
	};
}

namespace Music
{
	struct {
		double consonance;
		double hue;
	} typedef ChordColorVector;

	// 定义 ChordColorVector 的 hash 方法，用于 unordered_map
	struct ChordColorVectorHash {
		size_t operator()(const ChordColorVector& a) const {
			return std::hash<double>()(a.consonance) ^ std::hash<double>()(a.hue);
		}
	};
}

class LoadConfig {

public:
	// 键盘映射
	static std::map<std::string, int>									KEYNAME2QTKEY;
	static std::unordered_map<int, int>									KEYMAP;

	// 音频参数
	static unsigned int													CHANNELS;
	static unsigned int													SAMPLE_RATE;
	static unsigned int													BUFFER_SIZE;

	// 合成器参数
	static std::array<unsigned int, 4>									ENVELOPE_PARAMETERS;
	static std::array<unsigned int, 13>									HARMONIC_INTENSITIES;

	// 和弦参数
	static std::unordered_map<std::string, std::array<bool, 12>>		CHORDNAME2PITCHCLASSES;
	static std::unordered_map<std::array<bool, 12>, std::string>		PITCHCLASSES2CHORDNAME;
	static std::unordered_map<std::string, Music::ChordColorVector>		CHORDNAME2COLORVECTOR;
	static std::unordered_map<int, std::string>							INTERVAL2NAME;

	// 配置文件
	json configJson;


	/*
		读取配置文件
	*/
	void readConfig() {
		// 打开文件
		std::ifstream configFile("data/config.json");
		if (!configFile.is_open()) {
			// statusBar->showMessage("Failed to open config.json", 5000);
			std::cout << "[ERROR] Failed to open config.json" << std::endl;
			return;
		}
		// 读取键盘映射
		try {
			configFile >> configJson;
			configFile.close();
			for (auto& item : configJson["keySettingConfig"].items()) {
				int note = item.value();
				KEYMAP[KEYNAME2QTKEY[item.key()]] = item.value();
			}
			std::cout << "[INFO] successfully read config file" << std::endl;
		} catch (std::exception& e) {
			std::cout << "[ERROR] Failed to read config.json" << std::endl;
			std::cout << "[INFO] Writing default config.json" << std::endl;
			json tempConfigJson = json::parse(DEFAULT_CONFIG_STRING);
			configJson["keySettingConfig"] = tempConfigJson["keySettingConfig"];
			for (auto& item : configJson["keySettingConfig"].items()) {
				int note = item.value();
				KEYMAP[KEYNAME2QTKEY[item.key()]] = item.value();
			}
		}

		// 读取音频参数
		try {
			CHANNELS = configJson["audioConfig"]["channels"];
			SAMPLE_RATE = configJson["audioConfig"]["sampleRate"];
			BUFFER_SIZE = configJson["audioConfig"]["bufferSize"];
			std::cout << "[INFO] successfully read audio config" << std::endl;
		} catch (std::exception& e) {
			std::cout << "[ERROR] Failed to read config.json" << std::endl;
			std::cout << "[INFO] Writing default config.json" << std::endl;
			configJson["audioConfig"]["channels"] = 2;
			configJson["audioConfig"]["sampleRate"] = 44100;
			configJson["audioConfig"]["bufferSize"] = 1024;
			CHANNELS = 2; SAMPLE_RATE = 44100; BUFFER_SIZE = 1024;
		}

		// 读取合成器参数
		try {
			for (int i = 0; i < 4; ++i) {
				ENVELOPE_PARAMETERS[i] = configJson["synthConfig"]["envelopeParameters"][i];
			}
			for (int i = 0; i < 13; ++i) {
				HARMONIC_INTENSITIES[i] = configJson["synthConfig"]["harmonicIntensities"][i];
			}
			std::cout << "[INFO] successfully read synth config" << std::endl;
		} catch (std::exception& e) {
			std::cout << "[ERROR] Failed to read config.json" << std::endl;
			std::cout << "[INFO] Writing default config.json" << std::endl;
			configJson["synthConfig"]["envelopeParameters"] = { 8, 32, 96, 8 };
			configJson["synthConfig"]["harmonicIntensities"] = { 127, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10 };
			ENVELOPE_PARAMETERS = { 8, 32, 96, 8 };
			HARMONIC_INTENSITIES = { 127, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10 };
		}
	}


	/*
		读取和弦名及其音级集，色度向量（浮点，角度）
	*/
	void readChordAttr() {
		// 打开 data/chordAttr.json，读取和弦名及其音级集，色度向量（浮点，角度）
		std::ifstream chordAttrFile("data/ChordAttr.json");
		if (!chordAttrFile.is_open()) {
			std::cout << "[ERROR] Failed to open ChordAttr.json" << std::endl;
			QMessageBox::warning(nullptr, "Error", "Failed to open ChordAttr.json");
			// statusBar->showMessage("Failed to open chordAttr.json");
			return;
		}
		try {
			// 从文件读取和弦名及其音级集，色度向量（浮点，角度）
			json chordAttrJson;
			chordAttrFile >> chordAttrJson;
			chordAttrFile.close();
			for (auto it = chordAttrJson.begin(); it != chordAttrJson.end(); ++it) {
				// it.key() 是和弦名，it.value() 是一个数组，第一个元素是音级集，第二个元素是色度向量
				std::string chordName = it.key();
				std::array<bool, 12> pitchClasses;
				for (int i = 0; i < 12; ++i) {
					if (it.value()[0][i] == 1)			pitchClasses[i] = true;
					else								pitchClasses[i] = false;
				}
				Music::ChordColorVector chordColorVector = { it.value()[1][0], it.value()[1][1] };
				CHORDNAME2PITCHCLASSES[chordName] = pitchClasses;
				PITCHCLASSES2CHORDNAME[pitchClasses] = chordName;
				CHORDNAME2COLORVECTOR[chordName] = chordColorVector;
			}
		}
		catch (std::exception& e) {
			std::cout << "[ERROR] Failed to read ChordAttr.json" << std::endl;
		}
	}


	/*
		保存配置文件
	*/
	void saveConfig() {
		// 打开 data/config.json
		std::ofstream configFile("data/config.json");
		if (!configFile.is_open()) {
			// statusBar->showMessage("Failed to open config.json", 5000);
			std::cout << "[ERROR] Failed to open config.json" << std::endl;
			return;
		}
		// 写入
		try {
			std::cout << "[INFO] Writing config.json" << std::endl;
			configJson["audioConfig"]["channels"] = CHANNELS;
			configJson["audioConfig"]["sampleRate"] = SAMPLE_RATE;
			configJson["audioConfig"]["bufferSize"] = BUFFER_SIZE;
			for (int i = 0; i < 4; ++i) {
				configJson["synthConfig"]["envelopeParameters"][i] = ENVELOPE_PARAMETERS[i];
			}
			for (int i = 0; i < 13; ++i) {
				configJson["synthConfig"]["harmonicIntensities"][i] = HARMONIC_INTENSITIES[i];
			}
			// for (auto& item : KEYMAP) {
			// 	configJson["keySettingConfig"][item.first] = item.second;
			// }
			configFile << configJson;
			std::cout << "[INFO] Successfully wrote config.json" << std::endl;
		}
		// 写入失败，写入默认配置
		catch (std::exception& e) {
			std::cout << "[ERROR] Failed to write config.json" << std::endl;
			std::cout << "[INFO] Writing default config.json" << std::endl;
			json defaultConfigJson;
			std::string file = DEFAULT_CONFIG_STRING;
			defaultConfigJson = json::parse(file);
			configFile << defaultConfigJson;
		}
		// 关闭文件
		configFile.close();
	}

	LoadConfig() {
		readConfig();
		readChordAttr();
	}

	~LoadConfig() {
		saveConfig();
	}
};
#endif // !LOADCONFIG_H