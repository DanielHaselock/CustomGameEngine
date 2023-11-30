#pragma once
#include "Game/Component/AComponent.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/VK/VKMesh.h"

namespace Game::Component
{
	class CPModel : public AComponent
	{
	public:
		Rendering::Resources::Model* mModel = nullptr;
		std::string mName = "Empty";
		std::string mPath = "";

		std::string socketName = "";
		Maths::FMatrix4* socketMatrix = nullptr;
		Maths::FMatrix4 localSocketMatrix;
		Maths::FVector4 boneVertice;

		Rendering::Resources::Material* mMat = nullptr;
#ifdef NSHIPPING
		Rendering::Resources::Material* mMatEditor = nullptr;
#endif
		std::string mNameMat = "Empty";
		std::string mPathMat = "";

		std::map<Rendering::Resources::VK::VKMesh*, Rendering::Resources::Material*> mMeshMat;
		std::map<std::string, bool> loadedMat;

		bool defaultMat = true;

		bool canReceiveDecal = false;

		CPModel();
		CPModel(const CPModel& pOther);
		AComponent* clone() override;
		~CPModel() override = default;

		void setModel(const std::string& pName, const char* pModel);
		void setModelWithPath(const char* pModel);
		void setModelWithPathLua(const char* pPath);
		Rendering::Resources::Model* getModel() const;


		void setMat(const std::string& pName, const char* pModel);
		void setMat(const char* pModel);
		void setMatWithPathLua(const char* pPath);
		Rendering::Resources::Material* getMat() const;

		void setSocket(std::string pSocketName);
		void setMatrixSocket(void* pActor = nullptr);

		void setMeshMat(const std::string& pMesh, const std::string& pMat);

		Maths::FMatrix4 updateMat(Maths::FMatrix4& pVP, Maths::FMatrix4& pModel);
		Maths::FMatrix4 updateMatEditor(Maths::FMatrix4& pVP, Maths::FMatrix4& pModel);

		void draw(void* pCmd, Rendering::Data::Material* pPipeLine = nullptr);
		void drawShadow(void* pCmd);
		void drawEditor(void* pCmd, const Maths::Frustum& frustrum, Rendering::Data::Material* pPipeLine = nullptr);
		void drawEditorShadow(void* pCmd);

		void OnAwake() override {};
		void OnStart() override {};
		void OnDestroy() override {};
		void OnUpdate(float pDeltaTime) override {};

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;
	};
}