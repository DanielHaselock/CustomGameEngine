#include "Game/Component/CPAnimator.h"
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

CPAnimator::~CPAnimator()
{
}

AComponent* CPAnimator::clone()
{
	return new CPAnimator(*this);
}

void CPAnimator::updateModel()
{
	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;


	if (actor->mComponents.find(Utils::ComponentType::MeshRenderer) == actor->mComponents.end())
	{
		cpModelIdx = -1;
		return;
	}

	int i = 1;
	for (Game::Component::AComponent* model : actor->mComponents[Utils::ComponentType::MeshRenderer])
	{
		if (i == cpModelIdx)
		{
			mCPModel = (Game::Component::CPModel*)model;
			break;
		}
		i++;
	}

	if (mCPModel == nullptr)
		cpModelIdx = -1;
}

void CPAnimator::updateAnimationFromModel()
{
	if (!mPath.empty())
		setAnimation(mPath.c_str());
}

void CPAnimator::setAnimation(const std::string& pName, const char* pPath)
{
	if (mCPModel == nullptr)
	{
		if (cpModelIdx != -1)
			updateModel();
		if (mCPModel == nullptr)
			return;
	}

	Rendering::Resources::Animation* anim = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Animation>("Anim" + std::string(pPath), pPath, mCPModel->mModel);
	setAnimationClip(anim);
	mName = pName;
	mPath = pPath;
}

void CPAnimator::setAnimation(const char* pPath)
{
	if (pPath == nullptr)
	{
		setAnimationClip(nullptr);
		mName = "";
		mPath = "";
		return;
	}
	
	if (mCPModel == nullptr)
	{
		if (cpModelIdx != -1)
			updateModel();
		if (mCPModel == nullptr)
			return;
	}

	Rendering::Resources::Animation* anim = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Animation>("Anim" + std::string(pPath), pPath, mCPModel->mModel);
	setAnimationClip(anim);
	mPath = pPath;
}

void CPAnimator::setAnimationWithLua(const char* pPath)
{
	if (mCPModel == nullptr)
	{
		if (cpModelIdx != -1)
			updateModel();
		if (mCPModel == nullptr)
			return;
	}

	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	mPath = currentPath + pPath;

	Rendering::Resources::Animation* anim = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Animation>("Anim" + std::string(pPath), mPath, mCPModel->mModel);
	setAnimationClip(anim);
}

void CPAnimator::setBlendAnimation(const char* pPath)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	std::string path = currentPath + pPath;

	Rendering::Resources::Animation* aAnim =
		service(EngineCore::ResourcesManagement::ResourceManager)
		.create<Rendering::Resources::Animation>("Anim" + std::string(pPath), path, mCPModel->mModel);

	setBlendAnimationClip(aAnim);
}

void CPAnimator::setCrossFadeAnimation(const char* pPath, float pFadeSpeed)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	std::string path = currentPath + pPath;

	Rendering::Resources::Animation* aAnim =
		service(EngineCore::ResourcesManagement::ResourceManager)
		.create<Rendering::Resources::Animation>("Anim" + std::string(pPath), path, mCPModel->mModel);

	setFadeAnimationClip(aAnim, pFadeSpeed);
}

void CPAnimator::setAdditiveAnimation(const char* pPath)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	std::string path = currentPath + pPath;

	Rendering::Resources::Animation* aAnim =
		service(EngineCore::ResourcesManagement::ResourceManager)
		.create<Rendering::Resources::Animation>("Anim" + std::string(pPath), path, mCPModel->mModel);

	setAdditiveAnimationClip(aAnim);
}

void CPAnimator::OnStart()
{
	if (!isActive)
		return;

	if (!playOnAwake)
		return;
	reset();
	playing = true;
}

void CPAnimator::OnUpdate(float pDeltaTime)
{
	if (!isActive)
		return;

	if (mCPModel == nullptr || !playing)
		return;


	updateAnimation(pDeltaTime, loop);

	for (int i = 0; i < MAX_BONE; ++i)
	{
		mCPModel->mMat->mUnibuffer.mData.mFinalBonesMatrices[i] = mFinalPose.mFinalMatrix[i];
#ifdef NSHIPPING
		mCPModel->mMatEditor->mUnibuffer.mData.mFinalBonesMatrices[i] = mFinalPose.mFinalMatrix[i];
#endif
	}

	mCPModel->mMat->mUnibuffer.mData.mHasAnimation = 1;
#ifdef NSHIPPING
	mCPModel->mMatEditor->mUnibuffer.mData.mHasAnimation = 1;
#endif
}

void CPAnimator::OnPaused()
{
	if (!isActive)
		return;

	playing = !playing;
}

void CPAnimator::OnDestroy()
{
	reset();
	OnUpdate(0);
	playing = false;
}

void CPAnimator::play()
{
	if (!isActive)
		return;

	paused = false;
	playing = true;
	reset();
}

void CPAnimator::pause()
{
	if (!isActive)
		return;

	paused = true;
	playing = false;
}

void CPAnimator::stop()
{
	playing = false;
	reset();
}

void CPAnimator::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Animator);

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

	pWriter.Key("CPModelIdx");
	pWriter.Int(cpModelIdx);

	pWriter.EndObject();
}