
#include <QtCore/qstandardpaths.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qlabel.h>
#include <QtGui/qscreen.h>
#include <QtGui/qevent.h>
#include <QtCore/qsavefile.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qmessagebox.h>
#include <QtGui/qwindow.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qapplication.h>
#include <QtDocking/DockAreaWidget.h>
#include "Editor/Widget/WidgetEditor.h"
#include "Tools/Utils/Define.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include "Editor/Widget/WidgetMenuSperarator.h"
#include "Editor/Widget/WidgetNewScene.h"
#include "Maths/Utils.h"
#include "Maths/FVector3.h"
#include "Rendering/Renderer/VK/VKOffScreenRenderer.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Resources/VK/PipeLineBuilder.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Resources/Prefab.h"
#include "Rendering/Data/UniformData.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Resources/Texture.h"
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/UI.h"
#include "Game/SceneSys/SceneManager.h"
#include "EngineCore/EventSystem/InputManager.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "Scripting/ScriptInterpreter.h"
#include "Game/Component/CPSoundListener.h"
#include <fstream>
#include <rapidjson/document.h>
#include <Rendering/Resources/Font.h>

using namespace Editor::Widget;

Editor::Widget::WidgetEditor::WidgetEditor(Core::EditorApp* pApp, const std::string& pProjectPath, const std::string& pProjectName) :
	mApp(pApp),
    QMainWindow(nullptr),
	mLocation(pProjectPath.c_str(), pProjectName.c_str()),
	mSettings("Resources/Editor/Config/EditorEN.ini", QSettings::IniFormat),
	mProjectSettings(QString(pProjectPath.c_str()) + "/" + QString(pProjectName.c_str()) + "." + Define::PROJECT_EXTENSION, QSettings::IniFormat),
    mEngineApp(800, 800)
{
    provideService(Editor::Widget::WidgetEditor, *this);
    provideService(Data::ProjectLocation, mLocation);

	setTheme();
	setWindowDecoration();

	initWindowControlButtons();
    
    initMainToolBar();
    initCenterContent();
    service(EngineCore::Thread::ThreadPool).queueJob([this]
        {
            mScene->run();
        });

    service(EngineCore::Thread::ThreadPool).queueJob([this]
        {
            mEngineApp.run();
        });

    readProjectSettings();
    initSceneLabel();
    show();

    
    service(EngineCore::Thread::ThreadPool).queueJob([this] 
    {
        service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Font>("Resources/Engine/Fonts/Antonio-Bold.ttf", "Resources/Engine/Fonts/Antonio-Bold.ttf");
        loadResources(mLocation.mFolder);
    });

    service(EngineCore::EventSystem::InputManager).bindActionCallBack(  EngineCore::EventSystem::KState::PRESS,
                                                                        EngineCore::EventSystem::Key::ESCAPE, 
                                                                        [this] 
        {
            if (!pressOnce)
            {
                pressOnce = true;
            }
            else
                return;

            if (mEngineApp.mMouseLock)
            {
                mEngineApp.mMouseLock = false;
                mEngineApp.mWindow.changeCursorMode(true, false);
            }
            else
                quit(); 
        });

    service(EngineCore::EventSystem::InputManager).bindActionCallBack(EngineCore::EventSystem::KState::RELEASE,
        EngineCore::EventSystem::Key::ESCAPE,
        [this]
        {
 
            pressOnce = false;
            
        });
}

WidgetEditor::~WidgetEditor()
{
    delete mDockManager;
}

void WidgetEditor::setTheme()
{
	setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));
}

void WidgetEditor::setWindowDecoration()
{
	setMinimumSize(mSettings.value("MinWidth").toInt(), mSettings.value("MinHeight").toInt());
	setWindowFlags(Qt::Window | /*Qt::FramelessWindowHint |*/ Qt::CustomizeWindowHint);
}



void WidgetEditor::initWindowControlButtons()
{
	QLabel* icon = new QLabel();
	icon->setPixmap(QPixmap(mSettings.value("IconPath").toString()).scaled(mSettings.value("IconWidth").toInt(), mSettings.value("IconHeight").toInt()));
	icon->setObjectName("IconTitle");
	icon->setAlignment(Qt::AlignHCenter);
	menuBar()->setCornerWidget(icon, Qt::TopLeftCorner);

    
    createFileMenu();
    createViewMenu();


    QToolBar* windowManagement = addToolBar("windowManager");
    menuBar()->setCornerWidget(windowManagement);
    QAction* newAction = nullptr;

    QLabel* ProjetName = new QLabel(mLocation.mName, this);
    ProjetName->setObjectName("ProjectName");
    windowManagement->addWidget(ProjetName);

    const QIcon minimizeIcon(mSettings.value("MinimizeIcon").toString());
    newAction = createAction(minimizeIcon, mSettings.value("MinimizeName").toString(), mSettings.value("MinimizeTips").toString(), QKeySequence::UnknownKey, *windowManagement);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::minimize);

    mScreenState.mScreenState.init(mSettings.value("ResizeMaxIcon").toString(), mSettings.value("ResizeRestoreIcon").toString(), createAction(QIcon(), mSettings.value("ResizeName").toString(), mSettings.value("ResizeTips").toString(), QKeySequence::UnknownKey, *windowManagement));
    connect(mScreenState.mScreenState.mAction, &QAction::triggered, this, &WidgetEditor::maximize);

    const QIcon quitIcon(mSettings.value("QuitIcon").toString());
    newAction = createAction(quitIcon, mSettings.value("QuitName").toString(), mSettings.value("QuitTips").toString(), QKeySequence::Quit, *windowManagement);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::quit);
}

void WidgetEditor::createFileMenu()
{
    QMenu* mFileMenu = menuBar()->addMenu(mSettings.value("MenuFile").toString());
    QAction* newAction = nullptr;

    {
        WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSettings, mSettings.value("CategoryLevelOpen").toString(), mSettings.value("SeparatorIcon").toString(), mFileMenu);
        mFileMenu->addAction(sepa->mAction);
    }

    newAction = createAction(QIcon(mSettings.value("MenuFileNewMapIcon").toString()), mSettings.value("MenuFileNewMap").toString(), mSettings.value("MenuFileNewMapTips").toString(), QKeySequence::New, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::newSceneDialog);

    newAction = createAction(QIcon(mSettings.value("MenuFileOpenMapIcon").toString()), mSettings.value("MenuFileOpenMap").toString(), mSettings.value("MenuFileOpenMapTips").toString(), QKeySequence::Open, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::openScene);
    
    {
        WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSettings, mSettings.value("CategoryLevelSave").toString(), mSettings.value("SeparatorIcon").toString(), mFileMenu);
        mFileMenu->addAction(sepa->mAction);
    }

    newAction = createAction(QIcon(mSettings.value("MenuFileSaveMapIcon").toString()), mSettings.value("MenuFileSaveMap").toString(), mSettings.value("MenuFileSaveMapTips").toString(), QKeySequence::Save, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::save);

    newAction = createAction(QIcon(mSettings.value("MenuFileSaveAsMapIcon").toString()), mSettings.value("MenuFileSaveAsMap").toString(), mSettings.value("MenuFileSaveAsMapTips").toString(), QKeySequence::SaveAs, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::saveAs);

    newAction = createAction(QIcon(mSettings.value("MenuFileSaveAllMapIcon").toString()), mSettings.value("MenuFileSaveAllMap").toString(), mSettings.value("MenuFileSaveAllMapTips").toString(), QKeySequence::UnknownKey, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::save);

    {
        WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSettings, mSettings.value("CategoryProject").toString(), mSettings.value("SeparatorIcon").toString(), mFileMenu);
        mFileMenu->addAction(sepa->mAction);
    }

    newAction = createAction(QIcon(mSettings.value("MenuFileNewProjectIcon").toString()), mSettings.value("MenuFileNewProject").toString(), mSettings.value("MenuFileNewProjectTips").toString(), QKeySequence::UnknownKey, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::newProject);

    newAction = createAction(QIcon(mSettings.value("MenuFileOpenProjectIcon").toString()), mSettings.value("MenuFileOpenProject").toString(), mSettings.value("MenuFileOpenProjectTips").toString(), QKeySequence::UnknownKey, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::openProject);

    {
        WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSettings, mSettings.value("CategoryBuild").toString(), mSettings.value("SeparatorIcon").toString(), mFileMenu);
        mFileMenu->addAction(sepa->mAction);
    }

    newAction = createAction(QIcon(mSettings.value("MenuFileBuildIcon").toString()), mSettings.value("MenuFileBuild").toString(), mSettings.value("MenuFileBuildTips").toString(), QKeySequence::UnknownKey, *mFileMenu);
    connect(newAction, &QAction::triggered, this, [this]
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), mLocation.mFolder, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty())
            return;
        
        mConsole->toggleView(true);
        service(EngineCore::Thread::ThreadPool).queueJob([this, dir]
        {
            createBuild(dir);
        });
    });

    {
        WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSettings, mSettings.value("CategoryExit").toString(), mSettings.value("SeparatorIcon").toString(), mFileMenu);
        mFileMenu->addAction(sepa->mAction);
    }

    newAction = createAction(QIcon(mSettings.value("MenuFileExitIcon").toString()), mSettings.value("MenuFileExit").toString(), mSettings.value("MenuFileExitTips").toString(), QKeySequence::UnknownKey, *mFileMenu);
    connect(newAction, &QAction::triggered, this, &WidgetEditor::quit);
}

void WidgetEditor::createViewMenu()
{
    QMenu* mViewMenu = menuBar()->addMenu(mSettings.value("MenuView").toString());
    QAction* newAction = nullptr;

    newAction = createAction(QIcon(), mSettings.value("MenuViewGame").toString(), mSettings.value("MenuViewGameTips").toString(), QKeySequence::UnknownKey, *mViewMenu);
    connect(newAction, &QAction::triggered, this, [newAction, this] { mWindow->toggleView(newAction->isChecked()); });
    newAction->setCheckable(true);
    newAction->setChecked(true);
    viewActions["game"] = newAction;

    newAction = createAction(QIcon(), mSettings.value("MenuViewScene").toString(), mSettings.value("MenuViewSceneTips").toString(), QKeySequence::UnknownKey, *mViewMenu);
    connect(newAction, &QAction::triggered, this, [newAction, this] { mScene->toggleView(newAction->isChecked()); });
    newAction->setCheckable(true);
    newAction->setChecked(true);
    viewActions["scene"] = newAction;

    newAction = createAction(QIcon(), mSettings.value("MenuViewBrowser").toString(), mSettings.value("MenuViewBrowserTips").toString(), QKeySequence::UnknownKey, *mViewMenu);
    connect(newAction, &QAction::triggered, this, [newAction, this] { mContent->toggleView(newAction->isChecked()); });
    newAction->setCheckable(true);
    newAction->setChecked(true);
    viewActions["browser"] = newAction;

    newAction = createAction(QIcon(), mSettings.value("MenuViewConsole").toString(), mSettings.value("MenuViewConsoleTips").toString(), QKeySequence::UnknownKey, *mViewMenu);
    connect(newAction, &QAction::triggered, this, [newAction, this] { mConsole->toggleView(newAction->isChecked()); });
    newAction->setCheckable(true);
    newAction->setChecked(true);
    viewActions["console"] = newAction;

    newAction = createAction(QIcon(), mSettings.value("MenuViewHierarchy").toString(), mSettings.value("MenuViewHierarchyTips").toString(), QKeySequence::UnknownKey, *mViewMenu);
    connect(newAction, &QAction::triggered, this, [newAction, this] { mHierarchy->toggleView(newAction->isChecked()); });
    newAction->setCheckable(true);
    newAction->setChecked(true);
    viewActions["hierarchy"] = newAction;

    newAction = createAction(QIcon(), mSettings.value("MenuViewInspector").toString(), mSettings.value("MenuViewInspectorTips").toString(), QKeySequence::UnknownKey, *mViewMenu);
    connect(newAction, &QAction::triggered, this, [newAction, this] { mInspector->toggleView(newAction->isChecked()); });
    newAction->setCheckable(true);
    newAction->setChecked(true);
    viewActions["inspector"] = newAction;
}

void WidgetEditor::initSceneLabel()
{
    QString scenePath = mLocation.mFolder + "/" + mProjectSettings.value("StartingMap").toString() + "." + Define::SCENE_EXTENSION;
    mLevelLabel.init(this, mSettings, menuBar()->size().height() + 1, scenePath, mProjectSettings.value("StartingMap").toString().split("/").last());
    
    Game::SceneSys::SceneManager& sceneManager = service(Game::SceneSys::SceneManager);
    sceneManager.mProjectPath = Utils::qStringToStdString(mLocation.mFolder);
    sceneManager.loadScene(Utils::qStringToStdString(scenePath).c_str());
    parseMap(Utils::qStringToStdString(scenePath));
}

void WidgetEditor::initMainToolBar()
{
    QToolBar* mainToolBar = addToolBar("toolbar");
    mainToolBar->setFloatable(false);
    mainToolBar->setMovable(false);
    mainToolBar->setFixedHeight(30);
    mainToolBar->setObjectName("MainToolBar");

    QAction* action = nullptr;
    QAction* separator = nullptr;

    const QIcon SaveIcon(mSettings.value("SaveIcon").toString());
    action = mainToolBar->addAction(SaveIcon, "");
    connect(action, &QAction::triggered, this, &WidgetEditor::save);

    mainToolBar->addSeparator();

    QToolBar* playToolBar = new QToolBar();
    playToolBar->setObjectName("PlayButtonToolBar");
    mainToolBar->addWidget(playToolBar);


    mPlayState.init(mSettings.value("PlayIcon").toString(), mSettings.value("PauseIcon").toString(), playToolBar->addAction(""));
    mPlayState.setTips(mSettings.value("PlayTips").toString());
    connect(mPlayState.mAction, &QAction::triggered, this, [this] 
        {
            if (!playOnceClick)
            {
                mConsole->play();
                mPlaying = true;
                copyScene(true);
                service(EngineCore::Core::EngineApp).mMouseLock = true;
                service(EngineCore::Core::EngineApp).mInGameMouseLock = true;
                mPlaying = false;
                service(Scripting::ScriptInterpreter).RefreshAll();
            }

            play();
        });

    mPlayFrameState.init(mSettings.value("PlayFrameIcon").toString(), mSettings.value("PlayFrameIconActif").toString(), playToolBar->addAction(""));
    mPlayFrameState.setTips(mSettings.value("PlayFrameTips").toString());
    mPlayFrameState.setDisabled(true);
    connect(mPlayFrameState.mAction, &QAction::triggered, this, &WidgetEditor::playFrame);

    mStopState.init(mSettings.value("StopIcon").toString(), mSettings.value("StopActifIcon").toString(), playToolBar->addAction(""));
    mStopState.setTips(mSettings.value("StopTips").toString());
    mStopState.setDisabled(true);
    connect(mStopState.mAction, &QAction::triggered, this, [this]
        {
            mPlaying = false;
            copyScene(false);
            service(EngineCore::Core::EngineApp).mMouseLock = false;
            service(EngineCore::Core::EngineApp).mInGameMouseLock = false;
            mPlaying = true;
            stop();
        });

    const QIcon ToogleCameraIcon(mSettings.value("ToogleCameraIcon").toString());
    action = playToolBar->addAction(ToogleCameraIcon, "");
    action->setDisabled(true);

    mainToolBar->addSeparator();
}

void WidgetEditor::initCenterContent()
{
    QFlags<ads::CDockManager::eConfigFlag> flags = ads::CDockManager::eConfigFlag::DragPreviewIsDynamic | ads::CDockManager::eConfigFlag::DockAreaHasUndockButton;
    ads::CDockManager::setConfigFlags(flags);
    mDockManager = new ads::CDockManager(this);
    mDockManager->setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("DockingStyleSheet").toString()));

    //Window
    mWindow = new WidgetGameApp(&mEngineApp);
    connect(mWindow, &ads::CDockWidget::closed, this, [this]{ viewActions["game"]->setChecked(false); });

    mInspector = new WidgetInspectorApp(mSettings);
    connect(mInspector, &ads::CDockWidget::closed, this, [this] { viewActions["inspector"]->setChecked(false); });

    mHierarchy = new WidgetSceneHierarchyApp(mSettings, *mInspector);
    connect(mHierarchy, &ads::CDockWidget::closed, this, [this] { viewActions["hierarchy"]->setChecked(false); });

    mScene = new WidgetSceneApp(mSettings, mHierarchy->mTree);
    connect(mScene, &ads::CDockWidget::closed, this, [this] { viewActions["scene"]->setChecked(false); });

    mContent = new WidgetContentBrowserApp(mSettings, mLocation);
    connect(mContent, &ads::CDockWidget::closed, this, [this] { viewActions["browser"]->setChecked(false); });

    mConsole = new WidgetConsoleApp(mSettings);
    connect(mConsole, &ads::CDockWidget::closed, this, [this] { viewActions["console"]->setChecked(false); });
    
    mMaterial = new WidgetMaterialApp(mSettings);
    connect(mMaterial, &ads::CDockWidget::closed, this, [this] { mMaterial->close(); });
    
    mParticle = new WidgetParticleApp(mSettings);
    connect(mParticle, &ads::CDockWidget::closed, this, [this] { mParticle->close(); });

    mUI = new WidgetUIApp(mSettings);
    connect(mUI, &ads::CDockWidget::closed, this, [this] { mUI->close(); });

    mModelView = new WidgetModelViewerApp(mSettings);
    
    
    
    //Centering
    ads::CDockAreaWidget* areaCentral = mDockManager->addDockWidget(ads::CenterDockWidgetArea, mScene);
    ads::CDockAreaWidget* areaLeft = mDockManager->addDockWidget(ads::LeftDockWidgetArea, mHierarchy, areaCentral);
    ads::CDockAreaWidget* areaRight = mDockManager->addDockWidget(ads::RightDockWidgetArea, mInspector, areaCentral);
    ads::CDockAreaWidget* areaBottom = mDockManager->addDockWidget(ads::BottomDockWidgetArea, mContent);

    mDockManager->addDockWidgetTabToArea(mModelView, areaCentral);
    mDockManager->addDockWidgetTabToArea(mWindow, areaCentral);
    mDockManager->addDockWidgetTabToArea(mMaterial, areaCentral);
    mDockManager->addDockWidgetTabToArea(mParticle, areaCentral);
    mDockManager->addDockWidgetTabToArea(mUI, areaCentral);

    mDockManager->addDockWidgetTabToArea(mConsole, areaBottom);

    // Resizewindow
    mDockManager->setSplitterSizes(areaCentral, { (int)(width() * 0.15), (int)(width() * 0.55), (int)(width() * 0.3) });
    mDockManager->setSplitterSizes(areaBottom, { (int)(height() * 0.70f), (int)(height() * 0.30f)});

    //Disabled window
    mScene->toggleView(true);
    mContent->toggleView(true);
    mModelView->toggleView(false);
    mMaterial->toggleView(false);
    mParticle->toggleView(false);
    mUI->toggleView(false);
}

void WidgetEditor::readProjectSettings()
{
    const QByteArray geometry = mProjectSettings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
            (availableGeometry.height() - height()) / 2);
    }
    else
        restoreGeometry(geometry);

    const bool maximized = mProjectSettings.value("Maximized", QVariant()).toBool();
    if (maximized)
        maximize();
}

void WidgetEditor::writeProjectSettings()
{
    mProjectSettings.setValue("geometry", saveGeometry());
    mProjectSettings.setValue("Maximized", mScreenState.mIsFullScreen);
}

QAction* WidgetEditor::createAction(const QIcon& pIcon, const QString& pTitle, const QString& pStatusTip, const QKeySequence::StandardKey& pShortcut, QWidget& pWidget)
{
    QAction* newAction = new QAction(pIcon, pTitle, this);
    newAction->setShortcuts(pShortcut);
    newAction->setStatusTip(pStatusTip);
    pWidget.addAction(newAction);
    return newAction;
}

bool WidgetEditor::maybeSave()
{
    if (!mProjectModified)
        return true;

    const QMessageBox::StandardButton ret  = QMessageBox::warning(this, tr("Application"), tr("The document has been modified.\n" "Do you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret)
    {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            return true;
    }

    return true;
}

void WidgetEditor::newSceneDialog()
{
    mApp->removeEventFilter(this);

    WidgetNewScene dialog(mLocation.mFolder);
    mApp->installEventFilter(&dialog);
    dialog.exec();
    if (dialog.result() == 1)
    {
        mLevelLabel.mFileName = dialog.mNewScene;
        mLevelLabel.mMapTitle->setText(QFileInfo(dialog.mNewScene).baseName());
    }

    mApp->installEventFilter(this);
}

void WidgetEditor::clearScene()
{
    service(Physics::Core::BulPhysicsEngine).mCollisionPairs.clear();
    //clear old scene
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;
    mEngineApp.mainThreadAction.pushFunction([this, &mtx, &ready, &cv]
        {
            Game::Component::CPCamera::mWorldCamera = nullptr;
            std::unique_lock<std::mutex> lck(mtx);
            ready = true;
            cv.notify_all();
        });

    {
        std::unique_lock<std::mutex> lck(mtx);
        while (!ready) cv.wait(lck);
    }
    mEngineApp.mPlaying = false;
    service(Physics::Core::BulPhysicsEngine).reset();
    ready = false;
    mScene->clearThreadAction.pushFunction([this, &mtx, &ready, &cv]
        {
            mScene->renderer->waitForCleanUp();
            mScene->mActors.clear();

            
            mScene->mTreeItem->mSelectedActor = nullptr;


            std::unique_lock<std::mutex> lck(mtx);
            ready = true;
            cv.notify_all();
        });
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (!ready) cv.wait(lck);
    }
    
    mInspector->updateInspector(nullptr);
    mHierarchy->clearTree();
    
    mHierarchy->mTree.clearSelection();
}

void WidgetEditor::openScene()
{
    std::string filter = "Level File (*." + std::string(Define::SCENE_EXTENSION) + ")";
    QFileDialog dialog(this, tr("Open Level"), mLocation.mFolder, tr(filter.c_str()));
    connect(&dialog, &QFileDialog::directoryEntered, [&dialog, this](QString path) 
        {
            if (!path.contains(mLocation.mFolder))
                dialog.setDirectory(mLocation.mFolder);
        });

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString fileName = dialog.selectedFiles().first();
    if (fileName.isEmpty())
        return;
    
    clearScene();

    mLevelLabel.mFileName = fileName;
    mLevelLabel.mMapTitle->setText(QFileInfo(fileName).baseName());

    service(Game::SceneSys::SceneManager).loadScene(Utils::qStringToStdString(fileName).c_str());
    mEngineApp.initScene();
    mScene->initLight();
    parseMap(Utils::qStringToStdString(fileName));
  
}

void WidgetEditor::openSceneWithPath(QString pMap)
{
    clearScene();

    mLevelLabel.mFileName = pMap;
    mLevelLabel.mMapTitle->setText(QFileInfo(pMap).baseName());

    service(Game::SceneSys::SceneManager).loadScene(Utils::qStringToStdString(pMap).c_str());
    mEngineApp.initScene();
    mScene->initLight();

    parseMap(Utils::qStringToStdString(pMap));
}

bool WidgetEditor::save()
{
    if (mEngineApp.mPlaying)
    {
        QMessageBox::warning(this, tr("Application"), tr("Can\'t save scene during simulation"), QMessageBox::Ok);
        return true;
    }

    if (mLevelLabel.mFileName.isEmpty())
        return saveAs();

    writeProjectSettings();
    return saveFile(mLevelLabel.mFileName);
}

bool WidgetEditor::saveAs()
{
    if (mEngineApp.mPlaying)
    {
        QMessageBox::warning(this, tr("Application"), tr("Can\'t save scene during simulation"), QMessageBox::Ok);
        return true;
    }

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    QString filter = "Level File (*." + QString(Define::SCENE_EXTENSION) + ")";
    dialog.setNameFilter(filter);
    dialog.setDefaultSuffix(Define::SCENE_EXTENSION);

    connect(&dialog, &QFileDialog::directoryEntered, [&dialog, this](QString path) {
        if (!path.contains(mLocation.mFolder))
            dialog.setDirectory(mLocation.mFolder);
        });

    if (dialog.exec() != QDialog::Accepted)
        return false;

    writeProjectSettings();
    return saveFile(dialog.selectedFiles().first());
}

bool WidgetEditor::saveFile(const QString& pFileName)
{
    QDir dir("Map");
    if (!dir.exists())
        dir.mkpath(".");

    QString errorMessage;
    QSaveFile file(QUrl::fromLocalFile(pFileName).toLocalFile());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        rapidjson::StringBuffer ss;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(ss);

        writer.StartObject();

        writer.Key("Actors");
        writer.StartArray();

        mHierarchy->mTree.serializeItem(writer);

        writer.EndArray();
        writer.EndObject();

        std::string s = ss.GetString();
        file.write(s.c_str());
        
        
        if (!file.commit())
        {
            errorMessage = tr("Cannot write file %1:\n%2.")
                .arg(QDir::toNativeSeparators(pFileName), file.errorString());
        }
    }
    else
    {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
            .arg(QDir::toNativeSeparators(pFileName), file.errorString());
    }

    if (!errorMessage.isEmpty())
    {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    return true;
}

void WidgetEditor::newProject()
{
    if (maybeSave())
    {
        writeProjectSettings();
        mDockManager->removeDockWidget(mHierarchy);
        {
            mEngineApp.close();
            std::unique_lock<std::mutex> lck(mEngineApp.closeMut);
            while (!mEngineApp.closed) mEngineApp.cv.wait(lck);
        }

        {
            mScene->close();
            std::unique_lock<std::mutex> lck(mScene->closeMut);
            while (!mScene->closed) mScene->cv.wait(lck);
        }
        delete mHierarchy;

        mEngineApp.mainThreadAction.flush();
        mScene->mainThreadAction.flush();
        mScene->clearThreadAction.flush();

        mModelView->close();
        mMaterial->close();
        mParticle->close();
        mUI->close();
        close();
        QApplication::exit(Define::EEditorState::OPEN_HUB);
        service(EngineCore::Thread::ThreadPool).stop();
    }
}

void WidgetEditor::openProject()
{
    QString filter = "Project Files (*." + QString(Define::PROJECT_EXTENSION) + ")";
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Project"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), filter);
    
    if (!filePath.isEmpty())
    {
        QFileInfo fileOpen(filePath);
        mNewProjectOpen.mFolder = fileOpen.absolutePath();
        mNewProjectOpen.mName = fileOpen.baseName();

        writeProjectSettings();
        mDockManager->removeDockWidget(mHierarchy);
        {
            mEngineApp.close();
            std::unique_lock<std::mutex> lck(mEngineApp.closeMut);
            while (!mEngineApp.closed) mEngineApp.cv.wait(lck);
        }

        {
            mScene->close();
            std::unique_lock<std::mutex> lck(mScene->closeMut);
            while (!mScene->closed) mScene->cv.wait(lck);
        }
        delete mHierarchy;

        mEngineApp.mainThreadAction.flush();
        mScene->mainThreadAction.flush();
        mScene->clearThreadAction.flush();

        mModelView->close();
        mMaterial->close();
        mParticle->close();
        mUI->close();
        close();
        QApplication::exit(Define::EEditorState::LOAD_EDITOR);
        service(EngineCore::Thread::ThreadPool).stop();
    }
}

void WidgetEditor::quit()
{
    if (hasQuit)
        return;

    hasQuit = true;
    if (maybeSave())
    {
        writeProjectSettings();

        mScene->mTreeItem->mSelectedActor = nullptr;

        mDockManager->removeDockWidget(mHierarchy);
        {
            std::unique_lock<std::mutex> lck(mEngineApp.closeMut);
            while (!mEngineApp.closed) { mEngineApp.close(); mEngineApp.cv.wait(lck); };
        }

        {
            std::unique_lock<std::mutex> lck(mScene->closeMut);
            while (!mScene->closed) { mScene->close(); mScene->cv.wait(lck); }
        }
    
        mScene->cleanupScene();
        mEngineApp.cleanupScene();

        delete mHierarchy;
        
        mEngineApp.mainThreadAction.flush();
        mEngineApp.clearThreadAction.flush();
        
        mScene->mainThreadAction.flush();
        mScene->clearThreadAction.flush();
        

        mModelView->close();
        mMaterial->close();
        mParticle->close();
        mUI->close();
        service(EngineCore::Thread::ThreadPool).stop();

        close();
        QApplication::exit(Define::CLOSE_EDITOR);
    }
}

void WidgetEditor::minimize()
{
    windowHandle()->showMinimized();
}

void WidgetEditor::maximize()
{
    mScreenState.mIsFullScreen = !mScreenState.mIsFullScreen;
    if (mScreenState.mIsFullScreen)
    {
        mScreenState.mOldPos = mapToGlobal(rect().topLeft());
        mScreenState.mOldSize = size();
        setWindowState(Qt::WindowMaximized);
        mScreenState.mScreenState.switchIcon(2);
    }
    else
    {
        move(mScreenState.mOldPos);
        resize(mScreenState.mOldSize);
        setWindowState(Qt::WindowActive);
        mScreenState.mScreenState.switchIcon(1);
    }
}

void WidgetEditor::copyScene(bool pSaveBeforeLoad)
{
    if (pSaveBeforeLoad)
        save();
    openSceneWithPath(mLevelLabel.mFileName);
}

void WidgetEditor::play()
{
    playOnceClick = true;

    mWindow->toggleView(true);

    
    if (mPaused)
    {
        mPlayState.switchIcon(2);
        mEngineApp.mPlaying = true;
        mEngineApp.mStart = true;
        mEngineApp.mStop = false;
        mEngineApp.mPaused = true;
        mPaused = false;

        mPlayFrameState.switchIcon(1);
        mPlayFrameState.setDisabled(true);

        mPlayState.setTips(mSettings.value("PauseTips").toString());
    }
    else
    {
        mPlayState.switchIcon(1);
        mEngineApp.mPlaying = false;
        mEngineApp.mPaused = true;
        mPaused = true;

        mPlayFrameState.switchIcon(2);
        mPlayFrameState.setDisabled(false);
        mPlayState.setTips(mSettings.value("ResumeTips").toString());
    }

    if (!mPlaying)
    {
        mConsole->toggleView(true);
        mPlaying = true;

        mStopState.switchIcon(2);
        mStopState.setDisabled(false);
    }
}

void WidgetEditor::playFrame()
{
    mEngineApp.playFrame();
}

void WidgetEditor::stop()
{
    playOnceClick = false;

    mPlayState.switchIcon(1);
    mPlaying = false;
    mPaused = true;

    mPlayFrameState.switchIcon(1);
    mPlayFrameState.setDisabled(true);

    mStopState.switchIcon(1);
    mStopState.setDisabled(true);

    mPlayState.setTips(mSettings.value("PlayTips").toString());

    mEngineApp.mPlaying = false;
    mEngineApp.mHasPlayed = false;
    mEngineApp.mStop = true;
    mEngineApp.mStart = false;
    mEngineApp.mPaused = false;

    mScene->toggleView(true);
}

bool WidgetEditor::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == menuBar() && event->type() == QEvent::MouseButtonPress)
    {
        if (mScreenState.mIsFullScreen)
        {
            mScreenState.mIsFullScreen = false;
            mScreenState.mScreenState.switchIcon(1);
        }
        windowHandle()->startSystemMove();
    }
    else if (event->type() == QEvent::KeyPress)
        Utils::sendQtInput((QKeyEvent*)event, EngineCore::EventSystem::KState::PRESS);
    else if (event->type() == QEvent::KeyRelease)
        Utils::sendQtInput((QKeyEvent*)event, EngineCore::EventSystem::KState::RELEASE);
    
    return QObject::eventFilter(obj, event);
}

void WidgetEditor::loadResources(const QString& pPath)
{
    const QList<QFileInfo> dirFileList = QDir(pPath).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);
    for (unsigned int i = 0; i < dirFileList.size(); i++)
    {
        if (dirFileList[i].isDir())
        {
            loadResources(dirFileList[i].absoluteFilePath());
            continue;
        }

        Tools::Utils::PathParser::EFileType type = Utils::getFileType(dirFileList[i].suffix());
        switch (type)
        {
            case Tools::Utils::PathParser::EFileType::UNKNOWN:
                break;

            case Tools::Utils::PathParser::EFileType::MODEL:
                loadFile<Rendering::Resources::Model>(dirFileList[i].absoluteFilePath(), dirFileList[i].baseName());
                break;

            case Tools::Utils::PathParser::EFileType::TEXTURE:
                loadFile<Rendering::Resources::Texture>(dirFileList[i].absoluteFilePath(), dirFileList[i].baseName());
                break;

            case Tools::Utils::PathParser::EFileType::MATERIAL:
                loadFile<Rendering::Resources::Material>(dirFileList[i].absoluteFilePath(), dirFileList[i].baseName());
                break;

            case Tools::Utils::PathParser::EFileType::UI:
                loadFile<Rendering::Resources::UI>(dirFileList[i].absoluteFilePath(), dirFileList[i].baseName());
                break;

            case Tools::Utils::PathParser::EFileType::PREFAB:
                //loadFile<Rendering::Resources::Prefab>(dirFileList[i].absoluteFilePath(), dirFileList[i].baseName());
                break;

            case Tools::Utils::PathParser::EFileType::SCENE:
                service(Game::SceneSys::SceneManager).addScene(Utils::qStringToStdString(dirFileList[i].absoluteFilePath()).c_str());
                break;

            case Tools::Utils::PathParser::EFileType::SCRIPT:
                //loadFile<script>(dirFileList[i].absoluteFilePath());
                break;

            case Tools::Utils::PathParser::EFileType::FONT:
                //loadFile<Font>(dirFileList[i].absoluteFilePath());
                break;
        }
    }
}

void WidgetEditor::createPreview(std::string pFile, Rendering::Resources::Model* pModel)
{
    QDir dir(mLocation.mFolder + mSettings.value("previewFolder").toString());
    if (!dir.exists())
        dir.mkpath(".");
    std::string location = Utils::qStringToStdString(dir.absolutePath());
    SetFileAttributes(location.c_str(), FILE_ATTRIBUTE_HIDDEN);
    location += "/" + pFile + ".png";

    if (QFile::exists(location.c_str()))
    {
        if (pModel != nullptr)
            pModel->previewCreated = true;
        return;
    }

    //Create renderer and material, texture and uniform buffer
    Rendering::Renderer::VK::VKOffScreenRenderer renderer(800, 800);
    Rendering::Data::Material mat = Rendering::Renderer::Resources::VK::PipeLineBuilder()
        .initPipeLine("Resources/Editor/Shaders/CamModelVertex.vert.spv", "Resources/Editor/Shaders/CamModelFrag.frag.spv", renderer.mRenderPass, false);
    Rendering::Resources::Texture text("Resources/Engine/Textures/default.png");
    Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformData> unibuffer(VK_SHADER_STAGE_VERTEX_BIT);

    Rendering::Resources::TextureArray texArr(5);
    texArr.addTexture(text);
    texArr.addTexture(text);
    texArr.addTexture(text);
    texArr.addTexture(text);
    texArr.addTexture(text);
    texArr.createDescritorSet();

    //Bind to material
    mat.bindDescriptor("pbrTexture", texArr.mTextureSets);
    mat.bindDescriptor("ubo", unibuffer.mDescriptorSets);

    // Settup data view
    float max = Maths::fmax(Maths::fmax(pModel->mBox.mSize.x, pModel->mBox.mSize.y), pModel->mBox.mSize.z);
    float dist = max / tan(Maths::degreesToRadians(45) / 2);
    Maths::FVector3 position(0, pModel->mBox.mMin.y + (pModel->mBox.mSize.y / 2), -dist);
    unibuffer.mData = {
                        Maths::FMatrix4::createTransformMatrix({ 0, 0, 0 }, { -25, 0, 0 }, { 1, 1, 1 }),
                            Maths::FMatrix4::createPerspective(45, float(renderer.mWindowExtent.width) / float(renderer.mWindowExtent.height), 0.01f, dist * 2)
                            * Maths::FMatrix4::lookAt(position, position + Maths::FVector3::Forward, Maths::FVector3::Up)
                        };
    unibuffer.updateData();

    //Draw content
    renderer.beginFrame();
        mat.bindPipeLine(renderer.getCurrentCommandBuffer());
        pModel->draw(renderer.getCurrentCommandBuffer());
    renderer.endFrame();

    //Create image
    renderer.saveToPNG(location.c_str());


    pModel->previewCreated = true;
}

void WidgetEditor::openModel(QString pPath)
{
    mModelView->open(service(EngineCore::ResourcesManagement::ResourceManager).get<Rendering::Resources::Model>(Utils::qStringToStdString(pPath)));
}

void WidgetEditor::openMaterial(QString pPath)
{
    mMaterial->open(pPath);
}

void WidgetEditor::openParticle(QString pPath)
{
    mParticle->open(pPath);
}

void WidgetEditor::openUI(QString pPath)
{
    mUI->open(pPath);
}

std::string WidgetEditor::exec(const char* cmd, bool pPrintToConsole)
{
    char buffer[5000];
    std::string result = "";

    

    FILE* pipe = _popen(cmd, "r");
    if (!pipe)
        return ("Cant open File");
    try
    {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            result = buffer;
            result.pop_back();
            if (pPrintToConsole && (std::string::npos == result.find(".pdb") && std::string::npos == result.find("warning")))
                service(Editor::Widget::WidgetConsole).infoPrint(result.c_str());
        }
    }
    catch (...)
    {
        _pclose(pipe);
        throw;
    }

    _pclose(pipe);

    return result;
}

void WidgetEditor::createBuild(QString pOutput)
{
    //Copy project file
    Utils::copyAndReplaceFolderContents(mLocation.mFolder, pOutput + "/", false, pOutput);

    // Compile program
    std::string currentPath = std::filesystem::current_path().string();

    std::string vsLocation = exec(("\"\"" + currentPath + "/Resources/Editor/Build/vswhere.exe\"\" \" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath\"").c_str(), false);
    std::string version = exec(("\"cd " + vsLocation + "/VC/Tools/MSVC/ && dir /B /AD\"").c_str(), false);

    std::string vcvarsall = "\"" + vsLocation + "/VC/Auxiliary/Build/vcvarsall.bat\" x64";
    std::string cl = "\"" + vsLocation + "/VC/Tools/MSVC/" + version + "/bin/Hostx64/x64/cl\"";

    std::string vulkanPath = "C:/VulkanSDK/";
    std::string vulkanVersion = exec(("\"cd " + vulkanPath + " && dir /B /AD\"").c_str(), false);;

    std::string objectFile = " /Fo\"Resources/Editor/Build/\" ";
    std::string main = " /EHsc Resources/Editor/Build/main.cpp Resources/Editor/Build/include/spirv_reflect.cpp";
    std::string vulkanInclude = " /I " + vulkanPath + vulkanVersion + "/Include/ ";
    std::string include = " /I Resources/Editor/Build/include/ " + vulkanInclude;
    std::string libs = " Resources/Editor/Build/libs/*.lib user32.lib " + vulkanPath + vulkanVersion + "/Lib/vulkan-1.lib";
    std::string output = " /Fe\"" + Utils::qStringToStdString(pOutput) + "/" + Utils::qStringToStdString(mLocation.mName) + ".exe" + "\" ";
    
    std::string flags = " /std:c++17 /MD /O2 /Ob2 /DSHIPPING /INCREMENTAL:NO ";

    std::string compilationFile = objectFile + main + include + libs + output + flags;

    std::string result = exec(("\"" + vcvarsall + " && " + cl + compilationFile + "\"").c_str());

    //Copy dll
    Utils::copyAndReplaceFolderContents(QString((currentPath + "/Resources/Editor/Build/bin/").c_str()), pOutput + "/");

    //Copy assets
    QDir dir(pOutput + "/Resources");
    if (!dir.exists())
        dir.mkpath(".");
    Utils::copyAndReplaceFolderContents(QString((currentPath + "/Resources/Editor/Build/Resources/").c_str()), pOutput + "/Resources/");


    service(Editor::Widget::WidgetConsole).infoPrint("Build Complete");
}

#include <algorithm>
#include <execution>
void WidgetEditor::parseMap(std::string pName)
{
    std::ifstream file(pName);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    rapidjson::Document doc;
    bool result = doc.Parse(buffer.str().c_str()).HasParseError();
    if (result)
    {
        WidgetConsole::errorPrint("Can\"t parse map ", pName);
        return;
    }

    const rapidjson::Value& objects = doc["Actors"].GetArray();
    for (unsigned int i = 0; i < objects.Size(); i++)
        deserializeObjChild(nullptr, objects[i]);
}

Game::Data::Actor* WidgetEditor::parsePrefab(const char* pPath)
{
    std::ifstream file(Utils::qStringToStdString(mLocation.mFolder) + "/" + pPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    rapidjson::Document doc;
    bool result = doc.Parse(buffer.str().c_str()).HasParseError();
    if (result)
        return nullptr;

    return deserializeObjChild(nullptr, doc)->mActor;
}

WidgetGameObjectTreeItem* WidgetEditor::deserializeObjChild(void* pItem, const rapidjson::Value& pObject)
{
    WidgetGameObjectTreeItem* item = new WidgetGameObjectTreeItem(mSettings, pObject["Name"].GetString());
    if (pItem == nullptr)
        mHierarchy->mTree.append(item);
    else
    {
        ((WidgetGameObjectTreeItem*)pItem)->mActor->mChildActors.emplace_back(((WidgetGameObjectTreeItem*)item)->mActor);
        ((WidgetGameObjectTreeItem*)pItem)->appendRow(item);
        ((WidgetGameObjectTreeItem*)item)->mActor->mParent = ((WidgetGameObjectTreeItem*)pItem)->mActor;
    }

    const rapidjson::Value& tags = pObject["Tags"].GetArray();
    for (unsigned int i = 0; i < tags.Size(); i++)
        item->mActor->mTags.push_back(tags[i].GetString());

    const rapidjson::Value& objects = pObject["Components"].GetArray();
    for (unsigned int i = 0; i < objects.Size(); i++)
    {
        Game::Utils::ComponentType type = (Game::Utils::ComponentType)objects[i]["Type"].GetInt();
        if (type == Game::Utils::ComponentType::Transform)
        {
            Game::Component::CPTransform* transform = item->mActor->getTransform();
            
            transform->mLocalPosition = Maths::FVector3(objects[i]["Position"][0].GetDouble(), objects[i]["Position"][1].GetDouble(), objects[i]["Position"][2].GetDouble());
            transform->mLocalRotation = Maths::FQuaternion(objects[i]["Rotation"][0].GetDouble(), objects[i]["Rotation"][1].GetDouble(), objects[i]["Rotation"][2].GetDouble(), objects[i]["Rotation"][3].GetDouble());
            transform->mLocalScale = Maths::FVector3(objects[i]["Scale"][0].GetDouble(), objects[i]["Scale"][1].GetDouble(), objects[i]["Scale"][2].GetDouble());

            item->mActor->updateWorldTransform(transform);
        }
        else if (type == Game::Utils::ComponentType::MeshRenderer)
        {
            Game::Component::CPModel* model = new Game::Component::CPModel();
            model->isActive = objects[i]["Active"].GetBool();
            model->canReceiveDecal = objects[i]["ReceiveDecal"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                model->setModel(objects[i]["Name"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + strPath).c_str());
            
            if (!objects[i]["DefaultMat"].GetBool())
                model->setMat(objects[i]["Mat"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + objects[i]["MatPath"].GetString()).c_str());
            else
                model->setMat(objects[i]["Mat"].GetString(), objects[i]["MatPath"].GetString());
            
            item->mActor->addComponent(Game::Utils::ComponentType::MeshRenderer, model);
            
            model->setSocket(objects[i]["SocketName"].GetString());

            if (objects[i].HasMember("MeshMat"))
            {
                auto arr = objects[i]["MeshMat"].GetArray();
                for (int j = 0; j < arr.Size(); j++)
                    model->setMeshMat(arr[j]["MeshName"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + arr[j]["Mat"].GetString()).c_str());
            
                /*std::for_each(std::execution::par,
                    arr.Begin(),
                    arr.End(),
                    [this, model](const rapidjson::Value& arr) {
                        model->setMeshMat(arr["MeshName"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + arr["Mat"].GetString()).c_str());
                    });*/
            }

            item->mDatas->addModel(model);
        }
        else if (type == Game::Utils::ComponentType::Animator)
        {
            Game::Component::CPAnimator* anim = new Game::Component::CPAnimator();
            anim->mCPModel = nullptr;
            anim->isActive = objects[i]["Active"].GetBool();
            anim->playOnAwake = objects[i]["PlayOnAwake"].GetBool();
            anim->loop = objects[i]["Loop"].GetBool();
            anim->cpModelIdx = objects[i]["CPModelIdx"].GetInt();
            item->mActor->addComponent(Game::Utils::ComponentType::Animator, anim);

            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                anim->setAnimation(objects[i]["Name"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + strPath).c_str());

            item->mDatas->addAnimator(anim);
        }
        else if (type == Game::Utils::ComponentType::Particle)
        {
            Game::Component::CPParticle* particle = new Game::Component::CPParticle();
            particle->isActive = objects[i]["Active"].GetBool();
            particle->playOnAwake = objects[i]["PlayOnAwake"].GetBool();
            particle->loop = objects[i]["Loop"].GetBool();
            particle->playInEditor = objects[i]["PlayInEditor"].GetBool();
            particle->mPlaybackSpeed = objects[i]["PlayBackSpeed"].GetDouble();
            
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                particle->setParticle(objects[i]["Name"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + strPath).c_str());
            
            item->mActor->addComponent(Game::Utils::ComponentType::Particle, particle);
            item->mDatas->addParticle(particle);
        }
        else if (type == Game::Utils::ComponentType::Decal)
        {
            Game::Component::CPDecal* decal = new Game::Component::CPDecal(item->mActor->mValueChangedFromEditor);
            decal->isActive = objects[i]["Active"].GetBool();
            
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                decal->setDecal(objects[i]["Name"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + strPath).c_str());

            decal->mSize = Maths::FVector3(objects[i]["Size"][0].GetDouble(), objects[i]["Size"][1].GetDouble(), objects[i]["Size"][2].GetDouble());

            item->mActor->addComponent(Game::Utils::ComponentType::Decal, decal);
            item->mDatas->addDecal(decal);

            decal->updateDrawer();
        }
        else if (type == Game::Utils::ComponentType::Camera)
        {
            Game::Component::CPCamera* newCamera = new Game::Component::CPCamera(service(Rendering::Renderer::VK::VKRenderer).mWindow->mWidth,
                                                                                service(Rendering::Renderer::VK::VKRenderer).mWindow->mHeight,
                                                                                item->mActor->getTransform()->mLocalPosition,
                                                                                item->mActor->getTransform()->mLocalRotation,
                                                                                item->mActor->mValueChangedFromEditor);
            newCamera->isActive = objects[i]["Active"].GetBool();
            newCamera->mFOV = objects[i]["FOV"].GetDouble();
            newCamera->mNear = objects[i]["Near"].GetDouble();
            newCamera->mFar = objects[i]["Far"].GetDouble();
            newCamera->useSkyBox = objects[i]["UseSkyBox"].GetBool();

            bool defaultSky = objects[i]["defaultSky"].GetBool();
            if (newCamera->useSkyBox && !defaultSky)
            {
                const rapidjson::Value& arr = objects[i]["SkyBoxPath"].GetArray();
                std::string path = Utils::qStringToStdString(mLocation.mFolder);

                std::string left = path + arr[0].GetString();
                std::string right = path + arr[1].GetString();
                std::string top = path + arr[2].GetString();
                std::string button = path + arr[3].GetString();
                std::string front = path + arr[4].GetString();
                std::string back = path + arr[5].GetString();
                newCamera->skybox.setTexture(left, right, top, button, front, back);
            }
            else if (newCamera->useSkyBox && defaultSky)
                newCamera->skybox.setDefault();

            newCamera->updateProjection();
            
            item->mActor->addComponent(Game::Utils::ComponentType::Camera, newCamera);
            item->mDatas->addCamera(newCamera);

            newCamera->updateModel(item->mActor->getTransform()->mWorldMatrix);
        }
        else if (type == Game::Utils::ComponentType::BoxCollider)
        {
            Maths::FVector3 position = item->mActor->getTransform()->mWorldPosition;
            Maths::FVector3 size = Maths::FVector3(objects[i]["Size"][0].GetDouble(), objects[i]["Size"][1].GetDouble(), objects[i]["Size"][2].GetDouble()) /** item->mActor->getTransform()->mWorldScale*/;

            Game::Component::CPBoxCollider* newBody = new Game::Component::CPBoxCollider(position, size, (void*)item->mActor);
            newBody->isActive = objects[i]["Active"].GetBool();
            newBody->mType = (Physics::Data::TypeRigidBody)objects[i]["TypeCollision"].GetInt();
            newBody->mCenter = Maths::FVector3(objects[i]["Center"][0].GetDouble(), objects[i]["Center"][1].GetDouble(), objects[i]["Center"][2].GetDouble());
            newBody->mMass = objects[i]["Mass"].GetDouble();
            newBody->mBaseSize = size;
            newBody->mLockRotX = objects[i]["BlockRotationX"].GetBool();
            newBody->mLockRotY = objects[i]["BlockRotationY"].GetBool();
            newBody->mLockRotZ = objects[i]["BlockRotationZ"].GetBool();

            //newBody->recreateCollider();

            item->mActor->addComponent(Game::Utils::ComponentType::Collider, newBody);
            item->mDatas->addBoxCollider(newBody);

            newBody->UpdateTransform(*item->mActor->getTransform());

            if (mPlaying && newBody->mType == Physics::Data::TypeRigidBody::E_DYNAMIC)
                item->mActor->detachFromParent();
        }
        else if (type == Game::Utils::ComponentType::CapsuleCollider)
        {
            Maths::FVector3 position = item->mActor->getTransform()->mWorldPosition;
            float Radius = Maths::fmax(item->mActor->getTransform()->mWorldScale.x, item->mActor->getTransform()->mWorldScale.z);
            float Height = item->mActor->getTransform()->mWorldScale.y;
            
            Game::Component::CPCapsuleCollider* newBody = new Game::Component::CPCapsuleCollider(position, Radius, Height, (void*)item->mActor);
            newBody->isActive = objects[i]["Active"].GetBool();

            newBody->mType = (Physics::Data::TypeRigidBody)objects[i]["TypeCollision"].GetInt();
            //newBody->setType(newBody->mColliderType);
            //newBody->setMass(objects[i]["Mass"].GetDouble());
            newBody->mCenter = Maths::FVector3(objects[i]["Center"][0].GetDouble(), objects[i]["Center"][1].GetDouble(), objects[i]["Center"][2].GetDouble());
            newBody->mMass = objects[i]["Mass"].GetDouble();
            newBody->mRadius = objects[i]["Radius"].GetDouble();
            newBody->mHeight = objects[i]["Height"].GetDouble();
            newBody->mLockRotX = objects[i]["BlockRotationX"].GetBool();
            newBody->mLockRotY = objects[i]["BlockRotationY"].GetBool();
            newBody->mLockRotZ = objects[i]["BlockRotationZ"].GetBool();

            newBody->recreateCollider();

            item->mActor->addComponent(Game::Utils::ComponentType::Collider, newBody);
            item->mDatas->addCapsuleCollider(newBody);

            newBody->UpdateTransform(*item->mActor->getTransform());

            if (mPlaying && newBody->mType == Physics::Data::TypeRigidBody::E_DYNAMIC)
                item->mActor->detachFromParent();
        }
        else if (type == Game::Utils::ComponentType::ComplexCollider)
        {
            Maths::FVector3 position = item->mActor->getTransform()->mWorldPosition;
            Maths::FQuaternion rotation = item->mActor->getTransform()->mWorldRotation;
            Maths::FVector3 size = item->mActor->getTransform()->mWorldScale;

            Game::Component::CPComplexCollider* newBody = new Game::Component::CPComplexCollider(position, rotation, size, item->mActor->mComponents[Game::Utils::ComponentType::MeshRenderer], (void*)item->mActor);
            newBody->isActive = objects[i]["Active"].GetBool();
            newBody->mType = Physics::Data::TypeRigidBody::E_STATIC;
            //newBody->setType(newBody->mColliderType);
            //newBody->setMass(objects[i]["Mass"].GetDouble());
            newBody->mMass = objects[i]["Mass"].GetDouble();

            newBody->recreateCollider();

            item->mActor->addComponent(Game::Utils::ComponentType::Collider, newBody);
            item->mDatas->addComplexCollider(newBody);
        }
        else if (type == Game::Utils::ComponentType::Script)
        {
            Game::Component::CPScript* script = new Game::Component::CPScript();
            script->isActive = objects[i]["Active"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                script->setScript(objects[i]["Name"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + strPath).c_str());
            item->mActor->addComponent(Game::Utils::ComponentType::Script, script);

            item->mDatas->addScript(script);

            service(Scripting::ScriptInterpreter).registerScript(script);
        }
        else if (type == Game::Utils::ComponentType::Sound)
        {
            Game::Component::CPSound* sound = new Game::Component::CPSound();
            sound->isActive = objects[i]["Active"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                sound->setClip(objects[i]["Name"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + strPath).c_str());
            sound->mute = objects[i]["Mute"].GetBool();
            sound->playOnAwake = objects[i]["PlayOnAwake"].GetBool();
            sound->loop = objects[i]["Loop"].GetBool();
            sound->volume = objects[i]["Volume"].GetInt();
            sound->is3DSound = objects[i]["Is3DSound"].GetBool();
            sound->minDist = objects[i]["MinDist"].GetInt();
            item->mActor->addComponent(Game::Utils::ComponentType::Sound, sound);

            item->mDatas->addSound(sound);
        }
        else if (type == Game::Utils::ComponentType::SoundListener)
        {
            Game::Component::CPSoundListener* sound = new Game::Component::CPSoundListener();
            sound->isActive = objects[i]["Active"].GetBool();
            item->mActor->addComponent(Game::Utils::ComponentType::SoundListener, sound);

            item->mDatas->addSoundListener(sound);
        }
        else if (type == Game::Utils::ComponentType::UI)
        {
            Game::Component::CPUI* ui = new Game::Component::CPUI();
            ui->isActive = objects[i]["Active"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            ui->mOwner = item->mActor;
            if (!strPath.empty())
                ui->setUI(objects[i]["Name"].GetString(), (Utils::qStringToStdString(mLocation.mFolder) + strPath).c_str());
            item->mActor->addComponent(Game::Utils::ComponentType::UI, ui);

            item->mDatas->addUI(ui);
        }
        else if (type == Game::Utils::ComponentType::DirLight)
        {
            Game::Component::CPDirLight* dirLight = new Game::Component::CPDirLight();
            dirLight->isActive = objects[i]["Active"].GetBool();
            dirLight->mAttenuatuion = objects[i]["Attenuation"].GetDouble();
            dirLight->mColor = Maths::FVector3(objects[i]["Color"][0].GetDouble(), objects[i]["Color"][1].GetDouble(), objects[i]["Color"][2].GetDouble());
            dirLight->mCastShadow = objects[i]["CastShadow"].GetBool();
            dirLight->mBrightness = objects[i]["Brightness"].GetDouble();

            item->mActor->addComponent(Game::Utils::ComponentType::Light, dirLight);
            item->mDatas->addDirLight(dirLight);
        }
        else if (type == Game::Utils::ComponentType::PointLight)
        {
            Game::Component::CPPointLight* pointLight = new Game::Component::CPPointLight();
            pointLight->isActive = objects[i]["Active"].GetBool();
            pointLight->mAttenuatuion = objects[i]["Attenuation"].GetDouble();
            pointLight->mColor = Maths::FVector3(objects[i]["Color"][0].GetDouble(), objects[i]["Color"][1].GetDouble(), objects[i]["Color"][2].GetDouble());
            pointLight->mRadius = objects[i]["Radius"].GetDouble();
            pointLight->mBrightness = objects[i]["Brightness"].GetDouble();
            pointLight->mCastShadow = objects[i]["CastShadow"].GetBool();

            item->mActor->addComponent(Game::Utils::ComponentType::Light, pointLight);
#ifdef NSHIPPING
            pointLight->updateDrawer();
#endif

            item->mDatas->addPointLight(pointLight);
        }
        else if (type == Game::Utils::ComponentType::SpotLight)
        {
            Game::Component::CPSpotLight* spotLight = new Game::Component::CPSpotLight();
            spotLight->isActive = objects[i]["Active"].GetBool();
            spotLight->mAttenuatuion = objects[i]["Attenuation"].GetDouble();
            spotLight->mColor = Maths::FVector3(objects[i]["Color"][0].GetDouble(), objects[i]["Color"][1].GetDouble(), objects[i]["Color"][2].GetDouble());
            spotLight->mCutOff = objects[i]["CutOff"].GetDouble();
            spotLight->mOuterCutOff = objects[i]["OuterCutOff"].GetDouble();
            spotLight->mBrightness = objects[i]["Brightness"].GetDouble();
            spotLight->mCastShadow = objects[i]["CastShadow"].GetBool();
            spotLight->updateRad();

            item->mActor->addComponent(Game::Utils::ComponentType::Light, spotLight);
            item->mDatas->addSpotLight(spotLight);
        }
    }

    const rapidjson::Value& childs = pObject["Childs"].GetArray();
    for (unsigned int i = 0; i < childs.Size(); i++)
        deserializeObjChild(item, childs[i]);

    return item;
}