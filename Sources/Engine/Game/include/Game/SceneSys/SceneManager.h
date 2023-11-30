#pragma once
#include <list>
#include "Game/SceneSys/Scene.h"
#include <unordered_map>
#include <rapidjson/document.h>
#include "Rendering/Data/DeletionQueue.h"

namespace Game::SceneSys
{
	class SceneManager
	{
		public:
			~SceneManager();

			Scene* mCurrentScene = nullptr;
			std::string mProjectPath;
			std::unordered_map<std::string, Scene*> mScenes;
			Rendering::Data::DeletionQueue endJobParse;

			void loadScene(const char* pSceneName);
			void loadScene(const char* pSceneName, std::string pProjectPath);
			void loadSceneLua(const char* pSceneName);
			void addScene(const char* pPath);

			void parseMap(const char* pPath);
			Game::Data::Actor* parsePrefab(const char* pPath, bool addToScene = true, bool relativePath = true);
			Game::Data::Actor* deserializeObjChild(void* pParent, const rapidjson::Value& pObject, bool addToScene = true);
	};
}