#include "chord.h"



namespace MusicELements
{
	using json = nlohmann::json;

	std::unordered_map<int, std::string> Chord::INTERVAL2NAME = {
		{0, std::string("P8")},
		{1, std::string("m2")},
		{2, std::string("M2")},
		{3, std::string("m3")},
		{4, std::string("M3")},
		{5, std::string("P4")},
		{6, std::string("A4")},
		{7, std::string("P5")},
		{8, std::string("m6")},
		{9, std::string("M6")},
		{10, std::string("m7")},
		{11, std::string("M7")},
		{12, std::string("P8")}
	};

	std::unordered_map<std::string, std::array<bool, 12>> Chord::CHORDNAME2PITCHCLASSES = {};
	std::unordered_map<std::array<bool, 12>, std::string> Chord::PITCHCLASSES2CHORDNAME = {};
	std::unordered_map<std::string, ChordColorVector> Chord::CHORDNAME2COLORVECTOR = {};

	void Chord::readChordAttr() {
		// 打开 data/chordAttr.json，读取和弦名及其音级集，色度向量（浮点，角度）
		std::ifstream file("data/chordAttr.json");

		// 检查文件是否成功打开
		if (!file.is_open()) {
			std::cerr << "Failed to open the JSON file." << std::endl;
			return;
		}

		// 从文件读取 JSON 数据
		// 文件格式如下：
		//{
		//	"C:": [[1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0], [10, 55.0]],
		//	"C:m" : [[1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0], [10, 125.0]]
		//}
		json jsonData;
		file >> jsonData;

		// 关闭文件
		file.close();

		// 遍历 JSON 数据，并填充三个 map
		for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
			// it.key() 是和弦名，it.value() 是一个数组，第一个元素是音级集，第二个元素是色度向量
			std::string chordName = it.key();
			std::array<bool, 12> pitchClasses;
			for (int i = 0; i < 12; ++i) {
				if (it.value()[0][i] == 1)			pitchClasses[i] = true;
				else								pitchClasses[i] = false;
			}
			ChordColorVector chordColorVector =		{ it.value()[1][0], it.value()[1][1] };
			CHORDNAME2PITCHCLASSES[chordName] =		pitchClasses;
			PITCHCLASSES2CHORDNAME[pitchClasses] =	chordName;
			CHORDNAME2COLORVECTOR[chordName] =		chordColorVector;
		}
	}


	Chord::Chord(std::vector<Note*> init_notes) {
		if (init_notes.size() == 0) {
			throw std::runtime_error("Chord::Chord(std::vector<Note&> notes) : notes.size() == 0");
		}
		// 复制notes
		for (auto note : init_notes) {
			// 防止重复
			if (note != nullptr && std::find(this->notes.begin(), this->notes.end(), note) == this->notes.end()) {
				notes.push_back(note);
			}
		}
		// 排序（Note已经重载了<运算符）
		std::sort(notes.begin(), notes.end());
		getRootAndType();
	}

	Chord::~Chord() {}

	void Chord::getRootAndType() {
		// 初始化音级集
		pitchClasses.fill(false);
		for (auto note : notes) {
			pitchClasses[note->pitchClass] = true;
		}
		// 分类
		if (notes.size() == 1) {
			root = notes[0]->name + std::to_string(notes[0]->octave);
			type = "Note";
			name = "";
		}
		else if (notes.size() == 2) {
			type = "Interval";
			root = notes[0]->name;
			name = INTERVAL2NAME[notes[1]->midiNumber - notes[0]->midiNumber];
		}
		else {
			type = "Chord";
			std::string tn = PITCHCLASSES2CHORDNAME[pitchClasses];
			// 寻找冒号，冒号前为root，冒号后为name
			root = tn.substr(0, tn.find(":"));
			name = tn.substr(tn.find(":") + 1);
		}
		totalName = root + " " + name;
	}
}