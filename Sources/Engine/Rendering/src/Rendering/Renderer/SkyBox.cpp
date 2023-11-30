#include "Rendering/Renderer/SkyBox.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <Rendering/Resources/VK/PipeLineBuilder.h>
#include "EngineCore/Core/EngineApp.h"

#ifdef NSHIPPING
	#include "Editor/Widget/WidgetSceneApp.h"
#endif

using namespace Rendering::Renderer;

SkyBox::SkyBox() : mUniform(VK_SHADER_STAGE_VERTEX_BIT)
{
	EngineCore::ResourcesManagement::ResourceManager& manager = service(EngineCore::ResourcesManagement::ResourceManager);
	model = manager.create<Rendering::Resources::Model>("Resources/Engine/Models/Cube.fbx", "Resources/Engine/Models/Cube.fbx");
}

SkyBox::~SkyBox()
{
	if (texture != nullptr)
		delete texture;
}

void SkyBox::setDefault()
{
	paths.clear();

	EngineCore::ResourcesManagement::ResourceManager& manager = service(EngineCore::ResourcesManagement::ResourceManager);
	texture = new Rendering::Resources::TextureCube("Resources/Engine/Textures/defaultSky/left.png", "Resources/Engine/Textures/defaultSky/right.png",
		"Resources/Engine/Textures/defaultSky/top.png", "Resources/Engine/Textures/defaultSky/bottom.png", "Resources/Engine/Textures/defaultSky/front.png", "Resources/Engine/Textures/defaultSky/back.png");

	paths.push_back("Resources/Engine/Textures/defaultSky/left.png");
	paths.push_back("Resources/Engine/Textures/defaultSky/right.png");
	paths.push_back("Resources/Engine/Textures/defaultSky/top.png");
	paths.push_back("Resources/Engine/Textures/defaultSky/bottom.png");
	paths.push_back("Resources/Engine/Textures/defaultSky/front.png");
	paths.push_back("Resources/Engine/Textures/defaultSky/back.png");
}

void SkyBox::setTexture(const std::string& pFileNameLeft, const std::string& pFileNameRight,
	const std::string& pFileNameTop, const std::string& pFileNamebottom,
	const std::string& pFileNameFront, const std::string& pFileNameBack)
{
	service(EngineCore::Core::EngineApp).mainThreadAction.pushFunction([=] 
		{
			service(EngineCore::Core::EngineApp).rend->waitForCleanUp();

			if (texture != nullptr)
				delete texture;

			texture = new Rendering::Resources::TextureCube(pFileNameLeft, pFileNameRight, pFileNameTop, pFileNamebottom, pFileNameFront, pFileNameBack);
			isDefault = false;
		});
	
	paths.clear();
	paths.push_back(pFileNameLeft);
	paths.push_back(pFileNameRight);
	paths.push_back(pFileNameTop);
	paths.push_back(pFileNamebottom);
	paths.push_back(pFileNameFront);
	paths.push_back(pFileNameBack);

#ifdef NSHIPPING
	service(Editor::Widget::WidgetSceneApp).skyBox->setTextureEditor(pFileNameLeft, pFileNameRight, pFileNameTop, pFileNamebottom, pFileNameFront, pFileNameBack);
#endif
}

#ifdef NSHIPPING
void SkyBox::setTextureEditor(const std::string& pFileNameLeft, const std::string& pFileNameRight,
	const std::string& pFileNameTop, const std::string& pFileNamebottom,
	const std::string& pFileNameFront, const std::string& pFileNameBack)
{
	service(Editor::Widget::WidgetSceneApp).mainThreadAction.pushFunction([=]
	{
		service(Editor::Widget::WidgetSceneApp).renderer->waitForCleanUp();

		if (texture != nullptr)
			delete texture;

		texture = new Rendering::Resources::TextureCube(pFileNameLeft, pFileNameRight, pFileNameTop, pFileNamebottom, pFileNameFront, pFileNameBack);
		isDefault = false;
	});

	paths.clear();
	paths.push_back(pFileNameLeft);
	paths.push_back(pFileNameRight);
	paths.push_back(pFileNameTop);
	paths.push_back(pFileNamebottom);
	paths.push_back(pFileNameFront);
	paths.push_back(pFileNameBack);
}
#endif

void SkyBox::updateUniform(Maths::FMatrix4& pProj, Maths::FVector3& pos, float pFar)
{
	mUniform.mData.mProj = pProj;
	mUniform.mData.mModel = Maths::FMatrix4::createTransformMatrix(pos, { 0, 0, 0 }, { pFar, pFar, pFar });
	mUniform.updateData();
}

void SkyBox::draw(VkCommandBuffer cmd, Rendering::Data::Material& pMat)
{
	if (texture == nullptr)
		setDefault();

	pMat.bindDescriptor("samplerCubeMap", texture->mTextureSets);
	pMat.bindDescriptor("ubo", mUniform.mDescriptorSets);
	pMat.bindPipeLine(cmd);

	model->draw(cmd);
}