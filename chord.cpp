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
		// �� data/chordAttr.json����ȡ������������������ɫ�����������㣬�Ƕȣ�
		std::ifstream file("data/chordAttr.json");

		// ����ļ��Ƿ�ɹ���
		if (!file.is_open()) {
			std::cerr << "Failed to open the JSON file." << std::endl;
			return;
		}

		// ���ļ���ȡ JSON ����
		// �ļ���ʽ���£�
		//{
		//	"C:": [[1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0], [10, 55.0]],
		//	"C:m" : [[1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0], [10, 125.0]]
		//}
		json jsonData;
		file >> jsonData;

		// �ر��ļ�
		file.close();

		// ���� JSON ���ݣ���������� map
		for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
			// it.key() �Ǻ�������it.value() ��һ�����飬��һ��Ԫ�������������ڶ���Ԫ����ɫ������
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
		// ����notes
		for (auto note : init_notes) {
			// ��ֹ�ظ�
			if (note != nullptr && std::find(this->notes.begin(), this->notes.end(), note) == this->notes.end()) {
				notes.push_back(note);
			}
		}
		// ����Note�Ѿ�������<�������
		std::sort(notes.begin(), notes.end());
		getRootAndType();
	}

	Chord::~Chord() {}

	void Chord::getRootAndType() {
		// ��ʼ��������
		pitchClasses.fill(false);
		for (auto note : notes) {
			pitchClasses[note->pitchClass] = true;
		}
		// ����
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
			// Ѱ��ð�ţ�ð��ǰΪroot��ð�ź�Ϊname
			root = tn.substr(0, tn.find(":"));
			name = tn.substr(tn.find(":") + 1);
		}
		totalName = root + " " + name;
	}
}