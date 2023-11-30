#pragma once

#include <unordered_map>
#include <string>
#include <mutex>
#include "Rendering/Resources/IResource.h"


namespace EngineCore::ResourcesManagement
{
	class ResourceManager
	{
		public:
			std::recursive_mutex mManagerMutex;
			std::unordered_map<std::string, Rendering::Resources::IResource*> mManager;

			~ResourceManager();

			template <typename T, typename ...Args> T* create(const std::string& pStr, Args&... pArgs);
			template <typename T> T* get(const std::string& pStr);

			void deleteRes(const std::string& str);
	};


	template <typename T, typename ...Args> T* ResourceManager::create(const std::string& pStr, Args&... pArgs)
	{
		{
			std::unique_lock lock(mManagerMutex);

			std::unordered_map<std::string, Rendering::Resources::IResource*>::iterator it = mManager.find(pStr);
			if (it != mManager.end())
				return (T*)it->second;
		
			//mManager[pStr] = nullptr;
		}

		{
			T* value =  new T(pArgs...);

			std::unique_lock lock(mManagerMutex);

			if (mManager[pStr] == nullptr)
				mManager[pStr] = value;
			else
				delete value;
		}

		

		return (T*)mManager[pStr];
	}

	template <typename T> T* ResourceManager::get(const std::string& pStr)
	{
		std::unique_lock lock(mManagerMutex);

		std::unordered_map<std::string, Rendering::Resources::IResource*>::iterator it = mManager.find(pStr);
		if (it == mManager.end())
			return nullptr;

		return (T*)it->second;
	}
}