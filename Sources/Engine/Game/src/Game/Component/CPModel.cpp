#pragma once

#include <iostream>
#include "Game/Component/CPModel.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <Game/Utils/ComponentType.h>
#include "Tools/Utils/PathParser.h"
#include <filesystem>
#include "Game/SceneSys/SceneManager.h"
#ifdef NSHIPPING
	#include "Editor/Widget/WidgetEditor.h"
	#include "EngineCore/Service/ServiceLocator.h"
	#include "Editor/Utils/Utils.h"
#endif

using namespace Game::Component;


bool within(float min, float val, float max)
{
	return val >= min && val <= max;
}

bool test_AABB_against_frustum(const Maths::FMatrix4& MVP, const Rendering::Data::BoundingBox& aabb)
{
	for (size_t corner_idx = 0; corner_idx < 8; corner_idx++) {
		// Transform vertex
		Maths::FVector4 corner = MVP * aabb.corners[corner_idx];
		
		// Check vertex against clip space bounds
		if (within(-corner.w, corner.x, corner.w) || within(-corner.w, corner.y, corner.w) || within(0.0f, corner.z, corner.w))
			return true;

	}
	//return inside;
	return false;
}


CPModel::CPModel()
{
	Rendering::Resources::Material* master = 
		service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>("Resources/Engine/Material/WorldMaterial.mat", "Resources/Engine/Material/WorldMaterial.mat");

	mMat = master->getInstance();
#ifdef NSHIPPING
	mMatEditor = master->getInstance();
#endif

	mNameMat = "WorldMaterial";
	mPathMat = "Resources/Engine/Material/WorldMaterial.mat";
	defaultMat = true;
}

CPModel::CPModel(const CPModel& pOther)
{
	mModel = pOther.mModel;
	mName = pOther.mName;
	mPath = pOther.mPath;
	socketName = pOther.socketName;
	socketMatrix = pOther.socketMatrix;
	mMat = pOther.mMat;
#ifdef NSHIPPING
	mMatEditor = pOther.mMatEditor;
#endif
	mNameMat = pOther.mNameMat;
	mPathMat = pOther.mPathMat;
	defaultMat = pOther.defaultMat;
	canReceiveDecal = pOther.canReceiveDecal;

	if (defaultMat)
	{
		setMat("Resources/Engine/Material/WorldMaterial.mat");
		defaultMat = true;
	}
	else
		setMat(mPathMat.c_str());

	mMeshMat = pOther.mMeshMat;
	loadedMat = pOther.loadedMat;
}

AComponent* CPModel::clone()
{
	return new CPModel(*this);
}

void CPModel::setModel(const std::string& pName, const char* pModel)
{
	mMeshMat.clear();
	loadedMat.clear();

	mModel = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>(pModel, pModel);
	mName = pName;
	mPath = pModel;

	if (mModel == nullptr)
	{
		mName = "Unknown";
		mPath = "";
		return;
	}
}

void CPModel::setModelWithPath(const char* pModel)
{
	mMeshMat.clear();
	loadedMat.clear();

	if (pModel == nullptr)
	{
		mModel = nullptr;
		mName = "";
		mPath = "";
		return;
	}

	mModel = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>(pModel, pModel);
	mPath = pModel;

	if (mModel == nullptr)
		mPath = "";
}

void Game::Component::CPModel::setModelWithPathLua(const char* pPath)
{
	mMeshMat.clear();
	loadedMat.clear();

	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	mModel = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>(currentPath + pPath, currentPath + pPath);
	mPath = currentPath + pPath;

	if (mModel == nullptr)
		mPath = "";
}

Rendering::Resources::Model* CPModel::getModel() const
{
	return mModel;
}

void CPModel::setMat(const std::string& pName, const char* pMat)
{
	if (mNameMat == pName)
		return;

	Rendering::Resources::Material* master = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>(pMat, pMat);

	mMat = master->getInstance();
#ifdef NSHIPPING
	mMatEditor = master->getInstance();
#endif

	mNameMat = pName;
	mPathMat = pMat;

	if (mMat == nullptr)
	{
		mNameMat = "Unknown";
		mPathMat = "";
	}

	defaultMat = false;
}

void CPModel::setMat(const char* pMat)
{
	if (pMat == nullptr)
	{
		if (defaultMat)
			return;

		Rendering::Resources::Material* master =
			service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>("Resources/Engine/Material/WorldMaterial.mat", "Resources/Engine/Material/WorldMaterial.mat");

		mMat = master->getInstance();
#ifdef NSHIPPING
		mMatEditor = master->getInstance();
#endif

		mNameMat = "WorldMaterial";
		mPathMat = "Resources/Engine/Material/WorldMaterial.mat";
		defaultMat = true;
		return;
	}

	Rendering::Resources::Material* master;
	if (defaultMat)
		master = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>("Resources/Engine/Material/WorldMaterial.mat", "Resources/Engine/Material/WorldMaterial.mat");
	else
		master = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>(pMat, pMat);

	mMat = master->getInstance();
#ifdef NSHIPPING
	mMatEditor = master->getInstance();
#endif

	mPathMat = pMat;

	if (mMat == nullptr)
		mPathMat = "";

	defaultMat = false;
}

void CPModel::setMatWithPathLua(const char* pPath)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	mPath = currentPath + pPath;

	Rendering::Resources::Material* master = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>(mPath, mPath);

	mMat = master->getInstance();
#ifdef NSHIPPING
	mMatEditor = master->getInstance();
#endif
}

Rendering::Resources::Material* CPModel::getMat() const
{
	return mMat;
}

void CPModel::setSocket(std::string pSocketName)
{
	if (pSocketName.empty())
		return;

	socketName = pSocketName;
	if (mOwner == nullptr)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor->mParent == nullptr)
		return;

	setMatrixSocket(actor->mParent);
}

void CPModel::setMatrixSocket(void* pActor)
{
	Game::Data::Actor* actor = (Game::Data::Actor*)pActor;

	Game::Component::CPModel* animatedModel = nullptr;
	int boneId = -1;
	if (actor->mComponents.find(Game::Utils::ComponentType::MeshRenderer) != actor->mComponents.end())
		for (Game::Component::AComponent* component : actor->mComponents[Game::Utils::ComponentType::MeshRenderer])
		{
			Game::Component::CPModel* model = (Game::Component::CPModel*)component;
			if (model->mModel == nullptr)
				continue;

			for (auto& it : model->mModel->mBoneInfoMap)
				if (it.first._Equal(socketName))
				{
					boneId = it.second.mId;
					animatedModel = model;
					break;
				}
		}

	if (boneId != -1)
		for (auto& mesh : animatedModel->mModel->mMeshes)
		{
			for (int i = 0; i < mesh->mVertices.size(); i++)
			{
				if (mesh->mVertices[i].mBoneIDs.x == boneId || mesh->mVertices[i].mBoneIDs.y == boneId || mesh->mVertices[i].mBoneIDs.z == boneId || mesh->mVertices[i].mBoneIDs.w == boneId)
				{
					boneVertice = Maths::FVector4(mesh->mVertices[i].mPosition.x, mesh->mVertices[i].mPosition.y, mesh->mVertices[i].mPosition.z, 1);
					break;
				}
			}
		}

	if (boneId != -1)
		if (actor->mComponents.find(Game::Utils::ComponentType::Animator) != actor->mComponents.end())
			for (Game::Component::AComponent* component : actor->mComponents[Game::Utils::ComponentType::Animator])
			{
				Game::Component::CPAnimator* animation = (Game::Component::CPAnimator*)component;
				if (animation->mCPModel != animatedModel)
					continue;

				socketMatrix = &animation->mFinalPose.mFinalMatrix[boneId];
				return;
			}


	if (actor->mParent != nullptr)
		return setMatrixSocket(actor->mParent);

	socketMatrix = nullptr;
}

Maths::FMatrix4 CPModel::updateMat(Maths::FMatrix4& pVP, Maths::FMatrix4& pModel)
{
	if (mMat == nullptr)
		return pModel;

	if (socketMatrix != nullptr)
	{
		Game::Data::Actor* parent = ((Game::Data::Actor*)mOwner)->mParent;

		Maths::FVector4 finalVertice = (*socketMatrix) * boneVertice;
		finalVertice = parent->getTransform()->getWorldMatrix() * finalVertice;

		Game::Data::Actor* actual = ((Game::Data::Actor*)mOwner);

		Maths::FMatrix4 fm = Maths::FMatrix4::createTransformMatrixQ(
			{ finalVertice.x, finalVertice.y, finalVertice.z }, 
			parent->getTransform()->getWorldRotation(), 
			{1, 1, 1}) * actual->getTransform()->getLocalMatrix();
		mMat->setModelData(pVP, fm);

		return fm;
	}
	else
		mMat->setModelData(pVP, pModel);

	return pModel;
}

Maths::FMatrix4 CPModel::updateMatEditor(Maths::FMatrix4& pVP, Maths::FMatrix4& pModel)
{
#ifdef NSHIPPING
	if (mMatEditor == nullptr)
		return pModel;

	if (socketMatrix != nullptr)
	{
		Game::Data::Actor* parent = ((Game::Data::Actor*)mOwner)->mParent;

		Maths::FVector4 finalVertice = (*socketMatrix) * boneVertice;
		finalVertice = parent->getTransform()->getWorldMatrix() * finalVertice;

		Game::Data::Actor* actual = ((Game::Data::Actor*)mOwner);

		Maths::FMatrix4 fm = Maths::FMatrix4::createTransformMatrixQ({ finalVertice.x, finalVertice.y, finalVertice.z }, parent->getTransform()->getWorldRotation(), { 1, 1, 1 }) * actual->getTransform()->getLocalMatrix();

		mMatEditor->setModelData(pVP, fm);

		return fm;
	}
	else
		mMatEditor->setModelData(pVP, pModel);

#endif
	return pModel;
}

void CPModel::draw(void* pCmd, Rendering::Data::Material* pPipeLine)
{
	if (!isActive)
		return;

	Rendering::Resources::TextureArray* lastTexture = nullptr;

	if (mMat != nullptr && pPipeLine != nullptr)
	{
		mMat->bindMat(*pPipeLine);
		pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
	}

	if (mModel != nullptr)
	{
		//mModel->draw(pCmd);
		Maths::FMatrix4 mvp = mMat->mUnibuffer.mData.mViewProjection * mMat->mUnibuffer.mData.mModel;
		bool defaultUsed = true;
		for (Rendering::Resources::VK::VKMesh* mesh : mModel->mMeshes)
		{
			if (!test_AABB_against_frustum(mvp, mesh->mBoundingBox))
				continue;

			if (mMeshMat.find(mesh) == mMeshMat.end())
			{
				if (defaultUsed)
					mesh->draw((VkCommandBuffer)pCmd);
				else
				{
					if (mMat != nullptr && pPipeLine != nullptr)
					{
						mMat->bindMat(*pPipeLine);
						pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
						defaultUsed = true;
					}

					mesh->draw((VkCommandBuffer)pCmd);
				}
			}
			else
			{
				if (mMat != nullptr && pPipeLine != nullptr)
				{
					if (mMeshMat[mesh] == nullptr)
					{
						if (mMat != nullptr && pPipeLine != nullptr)
						{
							mMat->bindMat(*pPipeLine);
							pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
							defaultUsed = true;
						}

						mesh->draw((VkCommandBuffer)pCmd);
					}
					else
					{
						if (lastTexture != mMeshMat[mesh]->mTextureArray)
						{
							lastTexture = mMeshMat[mesh]->mTextureArray;

							pPipeLine->bindDescriptor("pbrTexture", mMeshMat[mesh]->mTextureArray->mTextureSets, 1);
							pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
							defaultUsed = false;
						}	
					}
					mesh->draw((VkCommandBuffer)pCmd);
				}
				else
					mesh->draw((VkCommandBuffer)pCmd);
			}
		}
	}
}

void CPModel::drawShadow(void* pCmd)
{
	if (!isActive)
		return;

	if (mModel != nullptr)
		mModel->draw(pCmd);
}

void CPModel::drawEditor(void* pCmd, const Maths::Frustum& frustrum, Rendering::Data::Material* pPipeLine)
{
	if (!isActive)
		return;

	Rendering::Resources::TextureArray* lastTexture = nullptr;

#ifdef NSHIPPING
	if (mMatEditor != nullptr && pPipeLine != nullptr)
	{
		mMatEditor->bindMat(*pPipeLine);
		pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
	}

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	
	if (mModel != nullptr)
	{
		//mModel->draw(pCmd);
		bool defaultUsed = true;
		Maths::FMatrix4 mvp = mMatEditor->mUnibuffer.mData.mViewProjection * mMatEditor->mUnibuffer.mData.mModel;
		for (Rendering::Resources::VK::VKMesh* mesh : mModel->mMeshes)
		{
			if (!test_AABB_against_frustum(mvp, mesh->mBoundingBox))
				continue;

			if (mMeshMat.find(mesh) == mMeshMat.end())
			{
				if (defaultUsed)
					mesh->draw((VkCommandBuffer)pCmd);
				else
				{
					if (mMatEditor != nullptr && pPipeLine != nullptr)
					{
						mMatEditor->bindMat(*pPipeLine);
						pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
						defaultUsed = true;
					}

					mesh->draw((VkCommandBuffer)pCmd);
				}
			}
			else
			{
				if (mMatEditor != nullptr && pPipeLine != nullptr)
				{
					if (mMeshMat[mesh] == nullptr)
					{
						if (mMatEditor != nullptr && pPipeLine != nullptr)
						{
							mMatEditor->bindMat(*pPipeLine);
							pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
							defaultUsed = true;
						}

						mesh->draw((VkCommandBuffer)pCmd);
					}
					else
					{
						if (lastTexture != mMeshMat[mesh]->mTextureArray)
						{
							lastTexture = mMeshMat[mesh]->mTextureArray;

							pPipeLine->bindDescriptor("pbrTexture", lastTexture->mTextureSets, 1);
							pPipeLine->bindPipeLine((VkCommandBuffer)pCmd);
							defaultUsed = false;
						}
						else
							int a = 0;
					}
					mesh->draw((VkCommandBuffer)pCmd);
				}
				else
					mesh->draw((VkCommandBuffer)pCmd);
			}
		}
	}
#endif
}

void CPModel::drawEditorShadow(void* pCmd)
{
	if (!isActive)
		return;

#ifdef NSHIPPING
	if (mModel != nullptr)
		mModel->draw(pCmd);
#endif
}

void CPModel::setMeshMat(const std::string& pMesh, const std::string& pMat)
{
	if (mModel == nullptr)
		return;

	if (loadedMat.find(pMesh) == loadedMat.end())
		loadedMat[pMesh] = true;
	else
		return;

	for (auto mesh : mModel->mMeshes)
		if (mesh->mName._Equal(pMesh))
			mMeshMat[mesh] = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>(pMat.c_str(), pMat)->getInstance();

	
	std::sort(mModel->mMeshes.begin(), mModel->mMeshes.end(),
		[this](Rendering::Resources::VK::VKMesh* a, Rendering::Resources::VK::VKMesh* b) -> bool
		{
			return mMeshMat[a] > mMeshMat[b];
		});
}

void CPModel::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::MeshRenderer);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Name");
	pWriter.String(mName.c_str());

	pWriter.Key("SocketName");
	pWriter.String(socketName.c_str());
	
	#ifdef NSHIPPING
		std::string currentPath = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder);
		pWriter.Key("Path");
		pWriter.String(mPath.empty() ? "": mPath.substr(currentPath.length()).c_str());
	#endif

	pWriter.Key("DefaultMat");
	pWriter.Bool(defaultMat);
	
	pWriter.Key("ReceiveDecal");
	pWriter.Bool(canReceiveDecal);

	pWriter.Key("Mat");
	pWriter.String(mNameMat.c_str());

#ifdef NSHIPPING
		pWriter.Key("MatPath");
		if (!defaultMat)
			pWriter.String(mPathMat.empty() ? "" : mPathMat.substr(currentPath.length()).c_str());
		else
			pWriter.String(mPathMat.c_str());
	
	
	pWriter.Key("MeshMat");
		pWriter.StartArray();
		for (auto it : mMeshMat)
		{
			if (it.second == nullptr)
				continue;

			pWriter.StartObject();
			pWriter.Key("MeshName");
			pWriter.String(it.first->mName.c_str());

			pWriter.Key("Mat");
			pWriter.String(it.second->mFilename.substr(currentPath.length()).c_str());
			pWriter.EndObject();
		}
		pWriter.EndArray();
#endif

	pWriter.EndObject();
}

