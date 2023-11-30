#pragma once
#include "Editor/Data/ArrowMode.h"
#include "Rendering/Resources/Model.h"
#include "Editor/Data/Actor.h"
#include <Rendering/Data/Material.h>
#include "Camera.h"

namespace Editor::Data
{
	struct Arrows
	{
		Editor::Data::ArrowMode mMode = Editor::Data::ArrowMode::ETranslation;

		Rendering::Resources::Model* mTranslation = nullptr;
		Rendering::Resources::Model* mRotation = nullptr;
		Rendering::Resources::Model* mScale = nullptr;

		Rendering::Data::Material* scene = nullptr;
		Rendering::Data::Material* picking = nullptr;

		Arrows(VkRenderPass pPass, VkRenderPass pOffScreenPass);
		~Arrows();

		Rendering::Resources::Model* getModel();
	};

	struct Arrow
	{
		int id = 0;
		Rendering::Resources::Model* mModel = nullptr;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataWithColor> mSceneUniform;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataWithId> mIdUniform;

		Arrow();
		void setSceneData(Maths::FMatrix4& pViewProj, Maths::FMatrix4 pModelMatrix, Maths::FVector3 pColor);
		void setPickingData(Maths::FMatrix4& pViewProj, Maths::FMatrix4 pModelMatrix);
	};

	class PickingArrow
	{
		public:
			static Maths::FVector3 red;
			static Maths::FVector3 green;
			static Maths::FVector3 blue;

			Arrows mAllArrow;
			Arrow forward;
			Arrow up;
			Arrow right;
			Maths::FVector3 axis;
			int leftRight = 0;
			Editor::Data::Actor* mActor = nullptr;

			bool isDrag = false;
			float sensitivity = 10;

			PickingArrow(VkRenderPass pPass, VkRenderPass pOffScreenPass);
			void setMode(Editor::Data::ArrowMode pMode);
			void drawScene(VkCommandBuffer pCmd, Maths::FMatrix4& viewProj, Maths::FVector3& pCamView);
			void drawPicking(VkCommandBuffer pCmd, Maths::FMatrix4& viewProj, Maths::FVector3& pCamView);

			void beginDrag(int pId, Editor::Data::Camera& pCamera);
			void moveDrag(Maths::FVector2 mousePos);
			void endDrag();

			void translate(float value);
			void rotate(float value);
			void scale (float value);
	};

}