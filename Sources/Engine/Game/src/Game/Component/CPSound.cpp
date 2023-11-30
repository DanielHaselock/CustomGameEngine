#include "Game/Component/CPSound.h"
#include <Game/Utils/ComponentType.h>
#include <EngineCore/Service/ServiceLocator.h>
#ifdef NSHIPPING
#include "Editor/Widget/WidgetEditor.h"
#endif
#include "Sounds/Core/SoundEngine.h"
#include <Game/Data/Actor.h>
#include "Game/SceneSys/SceneManager.h"
#include <filesystem>
#include "Tools/Utils/PathParser.h"

using namespace Game::Component;

CPSound::~CPSound()
{
	if (sound == nullptr)
		return;

	sound->stop();
	sound->drop();
	sound = nullptr;
}

AComponent* CPSound::clone()
{
	return new CPSound(*this);
}

void CPSound::setClip(const std::string& pName, const char* pClip)
{
	mName = pName;
	mPath = pClip;
}

void CPSound::setClip(const char* pPath)
{
	if (pPath == nullptr)
	{
		mName = "";
		mPath = "";
		return;
	}
	else
		mPath = pPath;
}

void CPSound::setClipWithLua(const char* pPath)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	mPath = currentPath + pPath;
}

void CPSound::OnStart()
{
	if (!isActive)
		return;

	if (!playOnAwake)
		return;
	play();
}

void CPSound::OnUpdate(float pDeltaTime)
{
	if (!isActive)
		return;

	if (is3DSound && sound != nullptr)
		sound->setPos(((Game::Data::Actor*)mOwner)->getTransform()->getWorldPosition());
}

void CPSound::OnPaused()
{
	if (!isActive)
		return;

	if (sound == nullptr)
		return;

	paused = !paused;
	sound->setIsPaused(paused);
}

void CPSound::OnDestroy()
{
	if (sound == nullptr)
		return;

	sound->stop();
	sound->drop();
	sound = nullptr;
}

void CPSound::play()
{
	if (!isActive)
		return;

	if (mPath.empty())
		return;

	float mappedVolume = float(volume) / 100.f;

	Sounds::Core::SoundEngine& engine = service(Sounds::Core::SoundEngine);
	engine.setVolume(mappedVolume);
	if (!is3DSound)
		sound = engine.playSound2D(mPath, loop);
	else
		sound = engine.playSound3D(mPath, ((Game::Data::Actor*)mOwner)->getTransform()->getWorldPosition(), loop, true, minDist);
	engine.setVolume(1);

	if (mute)
		sound->setVolume(0);
	else
		sound->setVolume(mappedVolume);
}

void CPSound::pause()
{
	if (sound == nullptr)
		return;
	sound->setIsPaused(paused);
}

void CPSound::setVolume(int pVolume)
{
	volume = pVolume;
	if (sound == nullptr)
		return;

	float mappedVolume = float(volume) / 100.f;
	sound->setVolume(mappedVolume);
}

void CPSound::setMute(bool pMute)
{
	mute = pMute;
	if (sound == nullptr)
		return;

	if (mute)
		sound->setVolume(0);
	else
		sound->setVolume(float(volume) / 100.f);
}

void CPSound::setLoop(bool pLoop)
{
	loop = pLoop;
	if (sound == nullptr)
		return;

	sound->setIsLooped(loop);
}

void CPSound::setMinDist(float pMinDist)
{
	minDist = pMinDist;
	if (sound == nullptr)
		return;

	sound->setMinDistance(minDist);
}

void CPSound::set3DSound(bool pIs3DSound)
{
	is3DSound = pIs3DSound;
}

void CPSound::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Sound);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Name");
	pWriter.String(mName.c_str());

#ifdef NSHIPPING
	std::string currentPath = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder);
	pWriter.Key("Path");
	pWriter.String(mPath.empty() ? "" : mPath.substr(currentPath.length()).c_str());
#endif

	pWriter.Key("Mute");
	pWriter.Bool(mute);

	pWriter.Key("PlayOnAwake");
	pWriter.Bool(playOnAwake);

	pWriter.Key("Loop");
	pWriter.Bool(loop);

	pWriter.Key("Volume");
	pWriter.Int(volume);

	pWriter.Key("Is3DSound");
	pWriter.Bool(is3DSound);

	pWriter.Key("MinDist");
	pWriter.Int(minDist);

	pWriter.EndObject();
}