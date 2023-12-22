#pragma once

#include <string>
#include <vector>
#include <map>

class Note {

public:
	static std::map<int, std::string> PITCH_CLASS2NAME;
	std::string name;                      // C, C#, D, D#, E, F, F#, G, G#, A, A#, B
	int pitchClass;                        // 0-11
	int midiNumber;                        // C4 = 60
	int octave;                            // A4 = 440 Hz
	double frequency;                      // Hz

	Note(int midiNumber) :
		midiNumber(midiNumber), pitchClass(midiNumber % 12), name(PITCH_CLASS2NAME[pitchClass]),
		octave(midiNumber / 12 - 1), frequency(440 * pow(2, (midiNumber - 69) / 12)) {}
	Note() = default;
	Note(const std::string& name, int pitchClass, int midiNumber, int octave, double frequency)
		: name(name), pitchClass(pitchClass), midiNumber(midiNumber), octave(octave), frequency(frequency) {}
	~Note() {};

};