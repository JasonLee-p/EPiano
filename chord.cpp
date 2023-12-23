#include "chord.h"

namespace MusicELements
{

std::map<int, std::string> Chord::INTERVAL2NAME = {
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

Chord::~Chord() {
}

void Chord::getRootAndType() {
	if (notes.size() == 1) {
		root = notes[0]->name + std::to_string(notes[0]->octave);
		type = "Note";
		name = "";
	}
	else if (notes.size() == 2) {
		type = "Interval";
		root = notes[0]->name;
		name = INTERVAL2NAME[(notes[1] - notes[0]) % 12];
	}
	else {
		type = "Chord";
		root = notes[0]->name;
		// TODO
		name = "";
	}
	totalName = root + " " + name;
}

}