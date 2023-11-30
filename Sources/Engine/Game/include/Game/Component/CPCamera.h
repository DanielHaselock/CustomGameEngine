#pragma once
#include "AComponent.h"
#include "Rendering/Data/Camera.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"
#include "EngineCore/EventSystem/Event.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Resources/Texture.h"
#include "Maths/FQuaternion.h"
#include <Rendering/Renderer/SkyBox.h>
#include "Rendering/Resources/TextureArray.h"

namespace Rendering
{
	class LineDrawer;
}

namespace Game::Component
{
	struct CameraViewer
	{
		Rendering::Resources::Model* mModel = nullptr;

		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformData> mUniformBuffer;

		Rendering::LineDrawer* mFrustrum;
		Rendering::Resources::TextureArray* mTextureArray = nullptr;
		
		Maths::FMatrix4& mViewProj;


		Rendering::Data::Material* mMat = nullptr;
		Rendering::Resources::Texture mTexture;

		CameraViewer(Maths::FMatrix4& pViewProj);
		~CameraViewer();

		void updateModel(Maths::FMatrix4& pMatrix);

		void createFrustrum();
		void draw(void* pCmd);
		void drawOffScreen(void* pCmd);
	};

	class CPCamera : public AComponent, public Rendering::Data::Camera
	{
	public:
		static CPCamera* mWorldCamera;

	#ifdef NSHIPPING
		CameraViewer mCamViewer;
	#endif
		EngineCore::EventSystem::Event<Maths::FMatrix4>& mEvent;
		size_t IteratorPos;

		Maths::FQuaternion mAdditionalAngle = Maths::FQuaternion::Identity;
		Maths::FQuaternion mActorQuaternion = Maths::FQuaternion::Identity;
		float mAdditionalPitch = 0;
		float mAdditionalYaw = 0;

		bool useSkyBox = false;
		Rendering::Renderer::SkyBox skybox;

		CPCamera(int pWidth, int pHeight, Maths::FVector3& pPosition, Maths::FQuaternion& pRotation, EngineCore::EventSystem::Event<Maths::FMatrix4>& pOnValueChanged);
		CPCamera(CPCamera& pOther);
		~CPCamera();

#ifdef NSHIPPING
		void draw(void* pCmd);
		void drawOffScreen(void* pCmd);
		void updateProjection(Maths::FMatrix4& pProj);
#endif
		void updateProjection() override;

		void updateProjection(int pWidth, int pHeight);

		void updateModel(Maths::FMatrix4& pMatrix);

		AComponent* clone() override;

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;

		void CalculateAngles();

		Maths::FVector3 getForward();

		Maths::FVector3 getUp();

		Maths::FVector3 getRight();

		void AddYaw(float pNewYaw);

		void AddPitch(float pNewYaw);

		void addAngleUpDown(Maths::FVector3 pAxis, float pAmount);

		void addAxisAngle(Maths::FVector3 pAxis);

		void drawSkyBox(VkCommandBuffer cmd, Rendering::Data::Material& pMat);
	};
}

