#include "Game/Component/CPParticle.h"
#include <Game/Utils/ComponentType.h>
#include <EngineCore/Service/ServiceLocator.h>
#ifdef NSHIPPING
	#include "Editor/Widget/WidgetEditor.h"
#endif
#include "Game/SceneSys/SceneManager.h"
#include "Game/Data/Actor.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <filesystem>
#include "Tools/Utils/PathParser.h"

using namespace Game::Component;


AComponent* CPParticle::clone()
{
	return new CPParticle(*this);
}

void CPParticle::setParticle(const std::string& pName, const char* pPath)
{
	mName = pName;
	mPath = pPath;

	Rendering::Resources::ParticleSystem* master = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::ParticleSystem>(pPath, pPath);

	mPat = master->getInstance();

#ifdef NSHIPPING
	mPatEditor = master->getInstance(true);
#endif
}

void CPParticle::setParticle(const char* pPath)
{
	if (pPath == nullptr)
	{
		mName = "";
		mPath = "";
		mPat = nullptr;
		return;
	}
	
	mPath = pPath;
	Rendering::Resources::ParticleSystem* master = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::ParticleSystem>(pPath, pPath);

	mPat = master->getInstance();

#ifdef NSHIPPING
	mPatEditor = master->getInstance(true);
#endif
}

void CPParticle::setParticleWithLua(const char* pPath)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	mPath = currentPath + pPath;

	Rendering::Resources::ParticleSystem* master = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::ParticleSystem>(mPath, mPath);

	mPat = master->getInstance();

#ifdef NSHIPPING
	mPatEditor = master->getInstance(true);
#endif
}

void CPParticle::OnStart()
{
	if (!isActive)
		return;

	if (!playOnAwake || mPat == nullptr)
		return;
	
	playing = true;
}

void CPParticle::OnUpdate(float pDeltaTime)
{
	if (!isActive)
		return;

	if (!playing || mPat == nullptr || mStop)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;

	if (actor != nullptr && Game::Component::CPCamera::mWorldCamera != nullptr)
	{
		Game::Component::CPModel* model = actor->getModel();
		if (model != nullptr && model->socketMatrix != nullptr)
		{
			Maths::FVector4& pos = model->mMat->mUnibuffer.mData.mModel.data[3];
			
			mPat->update(pDeltaTime * mPlaybackSpeed, Maths::FVector3(pos.x, pos.y, pos.z), Game::Component::CPCamera::mWorldCamera->mPosition, loop);
			return;
		}

		if (actor->mParent != nullptr)
		{
			Game::Component::CPModel* Pmodel = actor->mParent->getModel();

			Maths::FVector3& vec3Pos = actor->getTransform()->mLocalPosition;
			Maths::FVector4 pos = Pmodel->mMat->mUnibuffer.mData.mModel * Maths::FVector4(vec3Pos.x, vec3Pos.y, vec3Pos.z, 1);

			mPat->update(pDeltaTime * mPlaybackSpeed, Maths::FVector3(pos.x, pos.y, pos.z), Game::Component::CPCamera::mWorldCamera->mPosition, loop);
			return;
		}
		
		mPat->update(pDeltaTime * mPlaybackSpeed, actor->getTransform()->mWorldPosition, Game::Component::CPCamera::mWorldCamera->mPosition, loop);
	}
}

void CPParticle::OnUpdateEditor(float pDeltaTime, Maths::FVector3& pCamPos)
{
	if (!isActive)
		return;

#ifdef NSHIPPING
	if (mPatEditor == nullptr || !playInEditor || mStop)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor != nullptr)
	{
		Game::Component::CPModel* model = actor->getModel();
		if (model != nullptr && model->socketMatrix != nullptr)
		{
			Maths::FVector4& pos = model->mMat->mUnibuffer.mData.mModel.data[3];

			mPatEditor->update(pDeltaTime * mPlaybackSpeed, Maths::FVector3(pos.x, pos.y, pos.z), pCamPos, true);
			return;
		}
		
		if (actor->mParent != nullptr)
		{
			Game::Component::CPModel* Pmodel = actor->mParent->getModel();

			Maths::FVector3& vec3Pos = actor->getTransform()->mLocalPosition;
			Maths::FVector4 pos = Pmodel->mMat->mUnibuffer.mData.mModel * Maths::FVector4(vec3Pos.x, vec3Pos.y, vec3Pos.z, 1);

			mPatEditor->update(pDeltaTime * mPlaybackSpeed, Maths::FVector3(pos.x, pos.y, pos.z), pCamPos, true);
			return;
		}

		mPatEditor->update(pDeltaTime * mPlaybackSpeed, actor->getTransform()->mWorldPosition, pCamPos, true);
	}
#endif
}

void CPParticle::OnPaused()
{
	if (!isActive)
		return;

	playing = !playing;
}

void CPParticle::OnDestroy()
{
	OnUpdate(0);
	playing = false;
}

void CPParticle::play()
{
	if (!isActive)
		return;

	paused = false;
	mStop = false;
	playing = true;
}

void CPParticle::pause()
{
	if (!isActive)
		return;

	paused = true;
	playing = false;
}

void CPParticle::stop()
{
	mStop = true;
}

void CPParticle::reset()
{
	mPat->spawnCount = 0;
	mPat->finished = false;
#ifdef NSHIPPING
	mPatEditor->spawnCount = 0;
	mPatEditor->finished = false;
	mPatEditor->beingDrawAtLeastOnce = false;
#endif

	mPat->beingDrawAtLeastOnce = false;
}

void CPParticle::resetAndPlay()
{
	reset();
	play();
}

void CPParticle::draw(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (!isActive)
		return;

	if (mPat == nullptr || mStop)
		return;

	mPat->draw(VkCommandBuffer(pCmd), pPipeLine, pViewProj, pView);
}

void CPParticle::drawEditor(void* pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
	if (!isActive)
		return;

#ifdef NSHIPPING
	if (mPatEditor == nullptr || mStop)
		return;

	mPatEditor->draw(VkCommandBuffer(pCmd), pPipeLine, pViewProj, pView);
#endif
}

void CPParticle::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Particle);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Name");
	pWriter.String(mName.c_str());

#ifdef NSHIPPING
	std::string currentPath = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder);
	pWriter.Key("Path");
	pWriter.String(mPath.empty() ? "" : mPath.substr(currentPath.length()).c_str());
#endif

	pWriter.Key("PlayOnAwake");
	pWriter.Bool(playOnAwake);

	pWriter.Key("Loop");
	pWriter.Bool(loop);

	pWriter.Key("PlayInEditor");
	pWriter.Bool(playInEditor);

	pWriter.Key("PlayBackSpeed");
	pWriter.Double(mPlaybackSpeed);

	pWriter.EndObject();
}