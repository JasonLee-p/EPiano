#pragma once
#include "note.h"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <unordered_map>

#ifndef CHORD_H
#define CHORD_H

namespace std {
	// ���� std::array<bool, 12> �� hash ����
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

namespace MusicELements
{

	struct {
		double consonance;
		double hue;
	} typedef ChordColorVector;

	// ���� ChordColorVector �� hash ����
	struct ChordColorVectorHash {
		size_t operator()(const ChordColorVector& a) const {
			return std::hash<double>()(a.consonance) ^ std::hash<double>()(a.hue);
		}
	};

	class Chord
	{
	public:
		static void readChordAttr();
		static std::unordered_map<std::string, std::array<bool, 12>>	CHORDNAME2PITCHCLASSES;
		static std::unordered_map<std::array<bool, 12>, std::string>	PITCHCLASSES2CHORDNAME;
		static std::unordered_map<std::string, ChordColorVector>		CHORDNAME2COLORVECTOR;
		static std::unordered_map<int, std::string>						INTERVAL2NAME;
		// ������Ϣ
		std::vector<Note*>		notes;
		std::string				root;
		std::string				type;
		std::string				name;
		std::string				totalName;
		std::array<bool, 12>	pitchClasses;	// ������
		std::vector<int>		intervals;		// ���̼�


		Chord(std::vector<Note*> init_notes);
		~Chord();

		void getRootAndType();
	};
}

#endif // !CHORD_H