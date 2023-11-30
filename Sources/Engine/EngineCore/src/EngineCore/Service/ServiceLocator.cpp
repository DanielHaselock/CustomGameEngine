#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/Thread/ThreadPool.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Physics/Core/BulPhysicsEngine.h"
#include <Game/SceneSys/SceneManager.h>


using namespace EngineCore::Service;

ServiceLocator ServiceLocator::instance;

void ServiceLocator::clearService()
{
	remove<EngineCore::Thread::ThreadPool>();
	remove<EngineCore::ResourcesManagement::ResourceManager>();
	remove<Game::SceneSys::SceneManager>();
	remove<Physics::Core::BulPhysicsEngine>();
	remove<Rendering::Renderer::VK::VKRenderer>();

	services.clear();
}

ServiceLocator::~ServiceLocator()
{
	clearService();
}