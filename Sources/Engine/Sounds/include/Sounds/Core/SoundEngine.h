#pragma once
#include <iostream>
#include <stdio.h>
#include <irrKlang/irrKlang.h>
#include "Maths/FVector3.h"
#include "Sounds/Data/Sound.h"

#pragma comment(lib, "irrKlang.lib")

namespace Sounds::Core
{

	class SoundEngine
	{
	public:
		irrklang::ISoundEngine* soundEngine = nullptr;

		SoundEngine();
		~SoundEngine();
		irrklang::ISoundEngine* getEngine();

		Data::Sound* playSound3D(const std::string& soundPath, const Maths::FVector3& position, bool loop, bool track = true, float minDist = 5, bool startPaused = false);
		Data::Sound* playSound2D(const std::string& soundPath, bool loop, bool track = true, bool startPaused = false);
		void setVolume(float volume);

		void setListenerPosition(const Maths::FVector3& position, const Maths::FVector3& lookDir);
	};
}