#pragma once
#include <Rendering/Resources/Model.h>
#include <Rendering/Data/Material.h>
#include <Maths/FVector4.h>
#include <Maths/FVector2.h>
#include "Rendering/Resources/Texture.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <functional>
namespace Rendering::Resources::UIResource
{
	enum class IUI
	{
		Button,
		Text,
		Image
	};

	class Rect
	{
		public:
			Maths::FVector2 corner;
			Maths::FVector2 ratioPos;
			Maths::FVector2 size;
			Maths::FVector2 ratio;

			bool contain(float x, float y);

			bool contain(Maths::FVector2 p);
	};

	class Style
	{
		public:
			Maths::FVector4 color = Maths::FVector4(1, 1, 1, 1);
			Rendering::Resources::Texture* texture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>("Resources/Engine/Textures/ui.png", "Resources/Engine/Textures/ui.png");
			bool hasTexture = false;
	};

	class IUIResource
	{
		public:
			std::string name;
			IUI mType;
			bool isHover = true;
			bool isHidden = false;
			int zOrder = 0;
			Rect position;
			Style hover;
			Style press;
			Style normal;

			Style* current = &normal;

			std::function<void()> mLuaFunctionHover = nullptr;
			std::function<void()> mLuaFunctionPress = nullptr;

			IUIResource(Rect pRect, IUI pType);
			IUIResource(const IUIResource& pUI);

			virtual ~IUIResource() {};

			bool contain(float x, float y);

			bool contain(Maths::FVector2 p);

			virtual void onHover() { isHover = true; current = &hover; updateData(); if (mLuaFunctionHover) { mLuaFunctionHover(); } }
			virtual void onEndHover() { isHover = false; current = &normal; updateData(); }

			virtual void onPress() { current = &press; updateData(); }
			virtual void onPressed() { current = &hover; updateData(); }

			virtual void updateData() {};

			virtual void updateData(Maths::FMatrix4 pProj, int pW, int pH) {}

			virtual void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pTextMat, Rendering::Resources::Model& pModel) {}


			void CallLuaDelegatePress();

			void setAllColor(Maths::FVector4 pNewColor);
			void setNormalColor(Maths::FVector4 pNewColor);
			void setHoverColor(Maths::FVector4 pNewColor);
			void setPressColor(Maths::FVector4 pNewColor);

			void setVisible(bool pIsVisible);

			virtual void setScale(Maths::FVector2 pNewScale);

			Maths::FVector2 getScale();
	};
}