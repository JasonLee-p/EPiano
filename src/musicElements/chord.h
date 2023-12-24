#pragma once
#include "note.h"
#include <algorithm>
#include <stdexcept>
#include <map>

#ifndef CHORD_H
#define CHORD_H


namespace MusicELements
{

	class Chord
	{
	public:
		static std::map<int, std::string> INTERVAL2NAME;
		// 基础信息
		std::vector<Note*> notes;
		std::string root;
		std::string type;
		std::string name;
		std::string totalName;
		std::vector<int> pitchClasses; // 音级集



		Chord(std::vector<Note*> init_notes);
		~Chord();

		void getRootAndType();
	};
}

#endif // !CHORD_H