#include "EngineCore/ResourceManagement/ResourceManager.h"


using namespace EngineCore::ResourcesManagement;

ResourceManager::~ResourceManager()
{
	for (std::unordered_map<std::string, Rendering::Resources::IResource*>::iterator it = mManager.begin(); it != mManager.end(); it++)
		delete it->second;
}

void ResourceManager::deleteRes(const std::string& pStr)
{
	std::unordered_map<std::string, Rendering::Resources::IResource*>::iterator it = mManager.find(pStr);
	if (it != mManager.end())
	{
		delete it->second;
		mManager.erase(it);
	}
}