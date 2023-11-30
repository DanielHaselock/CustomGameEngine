#pragma once
#include <QtCore/qsettings.h>
#include <QtWidgets/qmainwindow.h>
#include <QtDocking/DockManager.h>

#include "Editor/Data/ProjectLocation.h"
#include "Editor/Data/ActionIcon.h"
#include "Editor/Widget/WidgetSceneLabel.h"
#include "WidgetConsole.h"
#include "WidgetContentBrowserApp.h"
#include "WidgetSceneHierarchyApp.h"
#include "Editor/Core/EditorApp.h"

#include <Windows.h>

#include "Editor/Utils/Utils.h"
#include "EngineCore/Core/EngineApp.h"
#include "Tools/Utils/PathParser.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/Thread/ThreadPool.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "Editor/Widget/WidgetSceneApp.h"
#include "Editor/Widget/WidgetGameApp.h"
#include "Editor/Widget/WidgetModelViewerApp.h"
#include "Editor/Widget/WidgetInspectorApp.h"
#include "Editor/Widget/WidgetParticleApp.h"
#include <rapidjson/document.h>
#include "WidgetMaterialApp.h"
#include <Editor/Widget/WidgetUIApp.h>

namespace Editor::Widget
{
	struct ScreenState
	{
		bool mIsFullScreen = false;
		QPoint mOldPos;
		QSize mOldSize;
		Data::ActionIcon mScreenState;
	};


	class WidgetEditor : public QMainWindow
	{
		public:
			Core::EditorApp* mApp = nullptr;
			QSettings mSettings;
			QSettings mProjectSettings;
			Data::ProjectLocation mLocation;
			WidgetSceneLabel mLevelLabel;

			ScreenState mScreenState;

			Data::ActionIcon mPlayState;
			Data::ActionIcon mPlayFrameState;
			Data::ActionIcon mStopState;

			bool mProjectModified = false;
			Data::ProjectLocation mNewProjectOpen;

			bool mPlaying = false;
			bool mPaused = true;
			bool hasQuit = false;
			bool playOnceClick = false;
			bool pressOnce = false;


			std::map<QString, QAction*> viewActions;

			ads::CDockManager* mDockManager = nullptr;
			WidgetConsoleApp* mConsole = nullptr;
			WidgetContentBrowserApp* mContent = nullptr;
			WidgetSceneHierarchyApp* mHierarchy = nullptr;
			WidgetGameApp* mWindow = nullptr;
			WidgetSceneApp* mScene = nullptr;
			WidgetModelViewerApp* mModelView = nullptr;
			WidgetInspectorApp* mInspector = nullptr;
			WidgetMaterialApp* mMaterial = nullptr;
			WidgetParticleApp* mParticle = nullptr;
			WidgetUIApp* mUI = nullptr;

			EngineCore::Core::EngineApp mEngineApp;

			WidgetEditor() : mEngineApp(800, 800) {};
			WidgetEditor(Core::EditorApp* pApp, const std::string& pProjectPath, const std::string& pProjectName);
			~WidgetEditor();

			void setTheme();
			void setWindowDecoration();
			
			void initWindowControlButtons();
			void createFileMenu();
			void createViewMenu();
			void initSceneLabel();
			void initMainToolBar();
			void initCenterContent();
			void readProjectSettings();
			void writeProjectSettings();

			QAction* createAction(const QIcon& pIcon, const QString& pTitle, const QString& pStatusTip, const QKeySequence::StandardKey& pShortcut, QWidget& pWidget);
			bool maybeSave();

			//Qevent
			void newSceneDialog();
			void clearScene();
			void openScene();
			void openSceneWithPath(QString pMap);
			bool save();
			bool saveAs();
			bool saveFile(const QString& pFileName);
			void newProject();
			void openProject();
			void quit();
			void minimize();
			void maximize();
			void copyScene(bool pSaveBeforeLoad = false);
			void play();
			void playFrame();
			void stop();

			bool eventFilter(QObject* obj, QEvent* event) override;
			void loadResources(const QString& pPath);
			
			template<typename T>
			void loadFile(const QString& pPath, const QString& pFileName)
			{
				EngineCore::Thread::ThreadPool& pool = service(EngineCore::Thread::ThreadPool);

				pool.queueJob([this, pPath, pFileName]
				{
					EngineCore::ResourcesManagement::ResourceManager& manager = service(EngineCore::ResourcesManagement::ResourceManager);
					T* resource = manager.create<T>(Utils::qStringToStdString(pPath), Utils::qStringToStdString(pPath));

					if (std::is_same<T, Rendering::Resources::Model>::value)
						createPreview(Utils::qStringToStdString(pFileName), (Rendering::Resources::Model*)resource);
				});
			};

			void createPreview(std::string pFile, Rendering::Resources::Model* pModel);
			void openModel(QString pPath);
			void openMaterial(QString pPath);
			void openParticle(QString pPath);
			void openUI(QString pPath);

			std::string exec(const char* cmd, bool pPrintToConsole = true);
			void createBuild(QString pOutput);

			void parseMap(std::string pName);
			Game::Data::Actor* parsePrefab(const char* pPath);
			WidgetGameObjectTreeItem* deserializeObjChild(void* pItem, const rapidjson::Value& pObject);
	};
}