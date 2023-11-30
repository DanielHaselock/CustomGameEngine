#include <string>
#include <Windows.h>
#include <filesystem>

#include "Tools/Utils/PathParser.h"
#include "Tools/Utils/Define.h"
#include "Tools/Utils/String.h"
#include <iostream>

void updateWorkingDirectory(const std::string& pExecutablePath)
{
	if (!IsDebuggerPresent())
		std::filesystem::current_path(Tools::Utils::PathParser::getContainingFolder(pExecutablePath));
}

#ifdef NSHIPPING

	#include "Editor/Core/EditorApp.h"

	int main(int pArgc, char** pArgv);
	static void tryRun(Editor::Core::EditorApp& pApp, const std::string& pProjectPath, const std::string& pProjectName);

	#if !DEBUG
	INT WinMain(HINSTANCE pHInstance, HINSTANCE pHPrevInstance, PSTR pLpCmdLine, INT pNCmdShow)
	{
		main(__argc, __argv);
	}
	#endif

	Editor::Core::EditorApp* app = nullptr;
	int main(int pArgc, char** pArgv)
	{
		if (pArgc > 0)
			updateWorkingDirectory(pArgv[0]);

		bool ready = false;
		std::string projectPath;
		std::string projectName;

		if (app == nullptr)
		{
			int argcCopy = pArgc;
			app = new Editor::Core::EditorApp(argcCopy);
		}

		{
			if (pArgc < 2)
			{
				// No project file given as argument ==> Open the ProjectHub
				std::tie(ready, projectPath, projectName) = app->runHub();
			}
			else
			{
				// Project file given as argument ==> Open the project
				std::string projectFile = pArgv[1];

				if (Tools::Utils::PathParser::getExtension(projectFile) == Define::PROJECT_EXTENSION)
				{
					ready = true;
					projectPath = Tools::Utils::PathParser::getContainingFolder(projectFile);
					projectName = Tools::Utils::PathParser::getElementName(projectFile);
					Tools::Utils::String::replace(projectName, std::string(".") + Define::PROJECT_EXTENSION, "");
				}
			}
		}

		if (ready)
			tryRun(*app, projectPath, projectName);

		if (app != nullptr)
		{
			delete app;
			app = nullptr;
		}
		return EXIT_SUCCESS;
	}

	static void tryRun(Editor::Core::EditorApp& pApp, const std::string& projectPath, const std::string& projectName)
	{
		{

			switch (pApp.runEditor(projectPath, projectName))
			{
				case Define::EEditorState::CLOSE_EDITOR: break;
				case Define::EEditorState::OPEN_EDITOR: break;
				case Define::EEditorState::OPEN_HUB: main(0, nullptr); break;
				case Define::EEditorState::LOAD_EDITOR: break;
			};
		}
	}
#endif

#ifdef SHIPPING
	#include "EngineCore/Core/EngineApp.h"
	#include "Game/SceneSys/SceneManager.h"
	#include "EngineCore/Service/ServiceLocator.h"
	#include <filesystem>
	int main(int pArgc, char** pArgv)
	{
		if (pArgc > 0)
			updateWorkingDirectory(pArgv[0]);

		EngineCore::Core::EngineApp mEngineApp(800, 800);
		

		std::string currentPath = Tools::Utils::PathParser::makeNonWindowsStyle(std::filesystem::current_path().string()) + "/";

		service(Game::SceneSys::SceneManager).loadScene("Map/Default.map", currentPath);
		
		mEngineApp.mStart = true;
		mEngineApp.mPlaying = true;
		mEngineApp.rend->mFramebufferResized = true;
		mEngineApp.mInGameMouseLock = true;
		mEngineApp.run();

		service(Game::SceneSys::SceneManager).mCurrentScene->clear();

		mEngineApp.mainThreadAction.flush();
		mEngineApp.clearThreadAction.flush();
		return 0;
	}
#endif