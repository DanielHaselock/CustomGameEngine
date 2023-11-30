#include "Game/Component/CPCamera.h"
#include "Rendering/LineDrawer.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Resources/VK/PipeLineBuilder.h"
#include "Rendering/Resources/Texture.h"
#include <Game/Utils/ComponentType.h>
#include "Game/SceneSys/SceneManager.h"
#ifdef NSHIPPING
#include "Editor/Widget/WidgetSceneApp.h"
#endif

using namespace Game::Component;

CPCamera* CPCamera::mWorldCamera = nullptr;

CameraViewer::CameraViewer(Maths::FMatrix4& pViewProj)
	: mViewProj(pViewProj), mUniformBuffer(VK_SHADER_STAGE_VERTEX_BIT), 
	mTexture("Resources/Engine/Textures/default.png")
{	
	mModel = service(EngineCore::ResourcesManagement::ResourceManager)
				.create<Rendering::Resources::Model>("Camera", "Resources/Editor/Models/Camera.fbx");
	mFrustrum = new Rendering::LineDrawer(service(Rendering::Renderer::VK::VKRenderer));
	
	mMat = new Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Editor/Shaders/CamModelVertex.vert.spv", 
			"Resources/Editor/Shaders/CamModelFrag.frag.spv", 
			service(Rendering::Renderer::VK::VKRenderer).mRenderPass, 
			false));

	mTextureArray = new Rendering::Resources::TextureArray(5);
	mTextureArray->addTexture(mTexture);
	mTextureArray->addTexture(mTexture);
	mTextureArray->addTexture(mTexture);
	mTextureArray->addTexture(mTexture);
	mTextureArray->addTexture(mTexture); 
	mTextureArray->createDescritorSet();


	mMat->bindDescriptor("pbrTexture", mTextureArray->mTextureSets);
	mMat->bindDescriptor("ubo", mUniformBuffer.mDescriptorSets);
}

CameraViewer::~CameraViewer()
{
	delete mFrustrum;
	delete mMat;
	delete mTextureArray;
}

void CameraViewer::createFrustrum()
{
	mFrustrum->reset();

	Maths::FVector4 coord1 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(1, 1, -1, 1.f);
	coord1 = (coord1 / coord1.w) * 1;				
													
	Maths::FVector4 coord2 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(-1, 1, -1, 1.f);
	coord2 = (coord2 / coord2.w) * 1;				  
													  
	Maths::FVector4 coord3 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(1, -1, -1, 1.f);
	coord3 = (coord3 / coord3.w) * 1;				 
													 
	Maths::FVector4 coord4 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(-1, -1, -1, 1.f);
	coord4 = (coord4 / coord4.w) * 1;				  
													  
	Maths::FVector4 coord5 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(1, 1, 1, 1.f);
	coord5 = (coord5 / coord5.w) * 1;				  
													  
	Maths::FVector4 coord6 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(-1, 1, 1, 1.f);
	coord6 = (coord6 / coord6.w) * 1;				  
													  
	Maths::FVector4 coord7 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(1, -1, 1, 1.f);
	coord7 = (coord7 / coord7.w) * 1;				  
													  
	Maths::FVector4 coord8 = Maths::FMatrix4::inverse(mViewProj) * Maths::FVector4(-1, -1, 1, 1.f);
	coord8 = (coord8 / coord8.w) * 1;

	// near face
	mFrustrum->drawLine({ coord1.x, coord1.y, coord1.z }, { coord2.x, coord2.y, coord2.z }, { 1, 1, 1 });
	mFrustrum->drawLine({ coord1.x, coord1.y, coord1.z }, { coord3.x, coord3.y, coord3.z }, { 1, 1, 1 });
																							  
	mFrustrum->drawLine({ coord4.x, coord4.y, coord4.z }, { coord2.x, coord2.y, coord2.z }, { 1, 1, 1 });
	mFrustrum->drawLine({ coord4.x, coord4.y, coord4.z }, { coord3.x, coord3.y, coord3.z }, { 1, 1, 1 });
																							  
	// far face																				  
	mFrustrum->drawLine({ coord5.x, coord5.y, coord5.z }, { coord6.x, coord6.y, coord6.z }, { 1, 1, 1 });
	mFrustrum->drawLine({ coord5.x, coord5.y, coord5.z }, { coord7.x, coord7.y, coord7.z }, { 1, 1, 1 });
																							
	mFrustrum->drawLine({ coord8.x, coord8.y, coord8.z }, { coord6.x, coord6.y, coord6.z }, { 1, 1, 1 });
	mFrustrum->drawLine({ coord8.x, coord8.y, coord8.z }, { coord7.x, coord7.y, coord7.z }, { 1, 1, 1 });
																							  
	// Connection																			  
	mFrustrum->drawLine({ coord1.x, coord1.y, coord1.z }, { coord5.x, coord5.y, coord5.z }, { 1, 1, 1 });
	mFrustrum->drawLine({ coord2.x, coord2.y, coord2.z }, { coord6.x, coord6.y, coord6.z }, { 1, 1, 1 });
																							  
	mFrustrum->drawLine({ coord3.x, coord3.y, coord3.z }, { coord7.x, coord7.y, coord7.z }, { 1, 1, 1 });
	mFrustrum->drawLine({ coord4.x, coord4.y, coord4.z }, { coord8.x, coord8.y, coord8.z }, { 1, 1, 1 });
}

void CameraViewer::updateModel(Maths::FMatrix4& pMatrix)
{
	mUniformBuffer.mData.mModel = pMatrix;
	mUniformBuffer.updateData();
}

void CameraViewer::draw(void* pCmd)
{
	if (mModel != nullptr)
	{
		mMat->bindPipeLine((VkCommandBuffer)pCmd);
		mModel->draw(pCmd);
	}
	
	mFrustrum->flushLines();
}

void CameraViewer::drawOffScreen(void* pCmd)
{
	if (mModel != nullptr)
		mModel->draw(pCmd);
}

CPCamera::CPCamera(int pWidth, int pHeight, Maths::FVector3& pPosition, Maths::FQuaternion& pRotation, EngineCore::EventSystem::Event<Maths::FMatrix4>& pOnValueChanged)
	: Rendering::Data::Camera(pWidth, pHeight, pPosition, pRotation) 
#ifdef NSHIPPING
	, mCamViewer(mVp)
#endif	
	, mEvent(pOnValueChanged)
{
	IteratorPos = pOnValueChanged.subscribe(std::bind(&CPCamera::updateModel, this, std::placeholders::_1));
	
	CalculateAngles();
	mView = Maths::FMatrix4::lookAt(mPosition, mPosition + (mRotation * mAdditionalAngle * Maths::FVector3::Forward), mUp);
	updateProjection();
	updateViewProj();
#ifdef NSHIPPING
	mCamViewer.mUniformBuffer.mData.mViewProjection = mVp;
	mCamViewer.updateModel(Maths::FMatrix4::createTransformMatrix(pPosition, pRotation.eulerAngles(), Maths::FVector3::One));
	mCamViewer.createFrustrum();
#endif
}


CPCamera::CPCamera(CPCamera& pOther) : Rendering::Data::Camera(pOther)
#ifdef NSHIPPING
, mCamViewer(pOther.mVp)
#endif
, mEvent(pOther.mEvent)
{
}

Game::Component::CPCamera::~CPCamera()
{
	//mEvent.remove(IteratorPos);
}

void CPCamera::updateModel(Maths::FMatrix4& pMatrix)
{
	if (mOwner == nullptr)
		return;
		

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	Game::Component::CPTransform* transform = actor->getTransform();

	CalculateAngles();
	mActorQuaternion = transform->getWorldRotation();

	mView = Maths::FMatrix4::lookAt(transform->getWorldPosition(), transform->getWorldPosition() + (mActorQuaternion * mAdditionalAngle * Maths::FVector3::Forward), mUp);
	updateViewProj();
#ifdef NSHIPPING
	mCamViewer.updateModel(pMatrix);
	mCamViewer.createFrustrum();
#endif
}

AComponent* CPCamera::clone()
{
	return new CPCamera(*this);
}

#ifdef NSHIPPING
void CPCamera::draw(void* pCmd)
{
	if (!isActive)
		return;

	mCamViewer.draw(pCmd);
}

void CPCamera::drawOffScreen(void* pCmd)
{
	mCamViewer.drawOffScreen(pCmd);
}
#endif

void CPCamera::updateProjection()
{
	Camera::updateProjection();
#ifdef NSHIPPING
	mCamViewer.createFrustrum();
#endif
}

#ifdef NSHIPPING
void CPCamera::updateProjection(Maths::FMatrix4& pProj)
{
	mCamViewer.mUniformBuffer.mData.mViewProjection = pProj;
	mCamViewer.mUniformBuffer.updateData();

	mCamViewer.mFrustrum->mUniformBuffer.mData.mViewProjection = pProj;
	mCamViewer.mFrustrum->mUniformBuffer.updateData();
}
#endif

void CPCamera::updateProjection(int pWidth, int pHeight)
{
	Rendering::Data::Camera::updateProjection(pWidth, pHeight);
#ifdef NSHIPPING
	mCamViewer.createFrustrum();
#endif
}

void CPCamera::CalculateAngles()
{
	mForward = mActorQuaternion *  mAdditionalAngle * Maths::FVector3::Forward;
	mRight =mActorQuaternion * mAdditionalAngle  * Maths::FVector3::Right;
	mUp = mActorQuaternion * mAdditionalAngle  * Maths::FVector3::Up;
}

Maths::FVector3 CPCamera::getForward()
{
	return mForward;
}

Maths::FVector3 CPCamera::getUp()
{
	return mUp;
}

Maths::FVector3 CPCamera::getRight()
{
	return mRight;
}

void CPCamera::AddYaw(float pNewYaw)//deprecated
{
	mAdditionalYaw += pNewYaw;
	Maths::FVector3 yawVec(mAdditionalYaw, mAdditionalPitch, 0);

	Maths::FQuaternion quat(yawVec);

	mAdditionalAngle = quat;
}

void CPCamera::AddPitch(float pNewPitch) //deprecated
{
	mAdditionalPitch += pNewPitch;
	Maths::FVector3 pitchVec(mAdditionalYaw, mAdditionalPitch, 0);

	Maths::FQuaternion quat(pitchVec);

	mAdditionalAngle = quat;
}

void Game::Component::CPCamera::addAngleUpDown(Maths::FVector3 pAxis, float pAmount)
{
	pAxis *= pAmount;
	Maths::FQuaternion quat = mRotation * Maths::FQuaternion(pAxis);
	
	mAdditionalAngle = quat;
}

void Game::Component::CPCamera::addAxisAngle(Maths::FVector3 pAxis)
{
	mAdditionalAngle = Maths::FQuaternion(pAxis);
}

void CPCamera::drawSkyBox(VkCommandBuffer cmd, Rendering::Data::Material& pMat)
{
	if (!isActive)
		return;

	if (useSkyBox)
	{
		skybox.updateUniform(viewProj(), ((Game::Data::Actor*)mOwner)->getTransform()->mWorldPosition, mFar);
		skybox.draw(cmd, pMat);
	}
}

void CPCamera::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Camera);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("FOV");
	pWriter.Double(mFOV);

	pWriter.Key("Near");
	pWriter.Double(mNear);

	pWriter.Key("Far");
	pWriter.Double(mFar);

	pWriter.Key("UseSkyBox");
	pWriter.Bool(useSkyBox);

	pWriter.Key("defaultSky");
	pWriter.Bool(skybox.isDefault);

	pWriter.Key("SkyBoxPath");
	pWriter.StartArray();
#ifdef NSHIPPING
	for (auto it = skybox.paths.begin(); it != skybox.paths.end(); it++)
	{
		if(it->find("Resources") != std::string::npos)
			skybox.isDefault = true;

		std::string currentPath = skybox.isDefault ? "" : service(Game::SceneSys::SceneManager).mProjectPath;
		pWriter.String(it->empty() ? "" : it->substr(currentPath.length()).c_str());
	}
#endif
	pWriter.EndArray();

	pWriter.EndObject();
}