#ifndef CHORD_H
#define CHORD_H

#include <QStatusBar>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include "loadConfig.h"


namespace Music
{

#define IS_AVAILABLE_NOTE_NUMBER(noteNumber) (noteNumber >= 0 && noteNumber <= 127)

class Note {

public:
	static std::map<int, std::string> PITCH_CLASS2NAME;

	std::string		name;				// C, C#, D, D#, E, F, F#, G, G#, A, A#, B
	int				pitchClass;			// 0-11
	int				midiNumber;			// C4 = 60
	int				octave;				// A4 = 440 Hz
	double			frequency;			// Hz

	Note(int midiNumber) : midiNumber(midiNumber) {
		// midiNumber��60ΪC4����C
		pitchClass = midiNumber % 12;
		name = PITCH_CLASS2NAME[pitchClass];
		octave = midiNumber / 12 - 1;
		frequency = 440.0 * pow(2.0, (midiNumber - 69.0) / 12.0);
	}
	Note() = default;
	~Note() {

	}

	// �Ƚ�����
	bool operator==(const Note& note) const {
		return this->midiNumber == note.midiNumber;
	}
	bool operator!=(const Note& note) const {
		return this->midiNumber != note.midiNumber;
	}
	bool operator<(const Note& note) const {
		return this->midiNumber < note.midiNumber;
	}
	bool operator>(const Note& note) const {
		return this->midiNumber > note.midiNumber;
	}
	bool operator<=(const Note& note) const {
		return this->midiNumber <= note.midiNumber;
	}
	bool operator>=(const Note& note) const {
		return this->midiNumber >= note.midiNumber;
	}

	// ��������
	int operator-(const Note& note) const {
		return this->midiNumber - note.midiNumber;
	}

	int operator-(const int& midiNumber) const {
		return this->midiNumber - midiNumber;
	}
};


class Chord
{
public:
	

	// ������Ϣ
	std::vector<Note*>		notes;
	std::string				root;
	std::string				type;
	std::string				name;
	std::string				totalName;
	std::array<bool, 12>	pitchClasses;	// ������
	std::vector<int>		intervals;		// ���̼�

	static void readChordAttr(QStatusBar* statusBar) {
		
	}


	Chord(std::vector<Note*> init_notes) {
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

	~Chord() {}

	void getRootAndType() {
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
			name = LoadConfig::INTERVAL2NAME[notes[1]->midiNumber - notes[0]->midiNumber];
		}
		else {
			type = "Chord";
			std::string tn = LoadConfig::PITCHCLASSES2CHORDNAME[pitchClasses];
			// Ѱ��ð�ţ�ð��ǰΪroot��ð�ź�Ϊname
			root = tn.substr(0, tn.find(":"));
			name = tn.substr(tn.find(":") + 1);
		}
		totalName = root + " " + name;
	}
};

}

#endif // !CHORD_H