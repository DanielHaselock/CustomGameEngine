#pragma once
#include "Game/Component/AComponent.h"
#include "Rendering/Resources/UI.h"
#include <string>
namespace Game::Component
{
	class CPUI : public AComponent
	{
	public:
		Rendering::Resources::UI* ui = nullptr;
		std::string mNameUI = "Empty";
		std::string mPathUI = "";

		CPUI();
		AComponent* clone() override;
		~CPUI() override = default;

		void setUI(const std::string& pName, const char* pUI);
		void setUIWithPath(const char* pUI);
		void setUIWithPathLua(const char* pUI);

		void updateUI(Maths::FMatrix4& pVP, int width, int height);
		
		void draw(void* pCmd, Rendering::Data::Material* pPipeLine = nullptr, Rendering::Data::Material* pPipeLineText = nullptr);

		bool contain(Maths::FVector2 p);
		Rendering::Resources::UIResource::IUIResource* containClick(Maths::FVector2 p);

		void OnAwake() override {};
		void OnStart() override {};
		void OnDestroy() override {};
		void OnUpdate(float pDeltaTime) override {};

		void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter) override;

		void DelegateLuaFunctionCP(int pHoverPress, std::function<void()> pFunction, std::string pName) { ui->DelegateLuaFunction(pHoverPress, pFunction, pName); };

	};
}