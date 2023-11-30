
#include "Sounds/Core/SoundEngine.h"

using namespace Sounds::Core;

SoundEngine::SoundEngine()
{
	soundEngine = irrklang::createIrrKlangDevice();
	soundEngine->setDefault3DSoundMinDistance(5);
}

SoundEngine::~SoundEngine()
{
	if (soundEngine != nullptr)
		soundEngine->drop();
}

irrklang::ISoundEngine* SoundEngine::getEngine()
{
	return soundEngine;
}

Sounds::Data::Sound* SoundEngine::playSound3D(const std::string& soundPath, const Maths::FVector3& position, bool loop, bool track, float minDist, bool startPaused)
{
	Sounds::Data::Sound* sound = (Sounds::Data::Sound*)soundEngine->play3D(soundPath.c_str(), irrklang::vec3df(position.x, position.y, position.z), loop, startPaused, track);
	sound->setMinDistance(minDist);
	return sound;
}

Sounds::Data::Sound* SoundEngine::playSound2D(const std::string& soundPath, bool loop, bool track, bool startPaused)
{
	return (Sounds::Data::Sound*)soundEngine->play2D(soundPath.c_str(), loop, startPaused, track);
}

void SoundEngine::setVolume(float volume)
{
	soundEngine->setSoundVolume(volume);
}

void SoundEngine::setListenerPosition(const Maths::FVector3& position, const Maths::FVector3& lookDir)
{
	soundEngine->setListenerPosition(irrklang::vec3df(position.x, position.y, position.z),
									irrklang::vec3df(lookDir.x, lookDir.y, lookDir.z));
}