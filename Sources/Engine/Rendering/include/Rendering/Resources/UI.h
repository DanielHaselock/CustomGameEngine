#pragma once
#include "Model.h"
#include <map>
#include <Maths/FMatrix4.h>
#include <Rendering/Data/Material.h>
#include <string>
#include "Rendering/Resources/IResource.h"
#include "Rendering/Resources/UI/IUIResource.h"
#include "rapidjson/document.h"

namespace Rendering::Resources
{
	class UI : public IResource
	{
		public:
			Rendering::Resources::Model* model;

			std::string mFilename;
			bool previewCreated = false;
			bool ready = false;
			std::map<int, std::list<Rendering::Resources::UIResource::IUIResource*>> mResources;

			

			UI(const std::string& pFileName);
			~UI() override;

			void update();
			Rendering::Resources::UIResource::Style loadStyle(const rapidjson::Value& obj);
			void updateSynch(void* owner = nullptr);

			void updateProj(Maths::FMatrix4 pProj, int w, int h);

			void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pMatText);

			bool contain(Maths::FVector2 p);
			Rendering::Resources::UIResource::IUIResource* containClick(Maths::FVector2 p);

			void DelegateLuaFunction(int pHoverPress, std::function<void()> pFunction, std::string pName);

			Rendering::Resources::UIResource::IUIResource* findResource(std::string pName);
	};
}