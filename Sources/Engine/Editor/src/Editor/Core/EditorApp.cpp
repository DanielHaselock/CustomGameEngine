#include "Editor/Core/EditorApp.h"
#include "Editor/Widget/WidgetProjectHub.h"
#include "Editor/Widget/WidgetEditor.h"
#include "Editor/Utils/Utils.h"

using namespace Editor::Core;

EditorApp::EditorApp(int& pArgc) : QApplication(pArgc, nullptr)
{
	QCoreApplication::setOrganizationName("CoorsLight");
	QCoreApplication::setApplicationName("Unreal CoorsLight Engine");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	setWindowIcon(QIcon("Resources/Editor/Icon/BeerBottle.png"));
}

std::tuple<bool, std::string, std::string> EditorApp::runHub()
{
	Widget::WidgetProjectHub hub;
	installEventFilter(&hub);
	switch (exec())
	{
		case Define::EEditorState::CLOSE_EDITOR: break;
		case Define::EEditorState::OPEN_EDITOR : 
			return std::tuple<bool, std::string, std::string>(true, Utils::qStringToStdString(hub.mCurrentProject->mFolder), Utils::qStringToStdString(hub.mCurrentProject->mName));
	};
	return std::make_tuple(false, "", "");
}

Define::EEditorState EditorApp::runEditor(const std::string& pProjectPath, const std::string& pProjectName)
{
	Define::EEditorState res;
	Data::ProjectLocation project;

	{
		Widget::WidgetEditor editor(this, pProjectPath, pProjectName);
		installEventFilter(&editor);

		res = (Define::EEditorState)exec();

		if (res == Define::LOAD_EDITOR)
			project = editor.mNewProjectOpen;
	}
	
	EngineCore::Service::ServiceLocator::instance.clearService();
	if (res == Define::LOAD_EDITOR)
		return runEditor(Utils::qStringToStdString(project.mFolder), Utils::qStringToStdString(project.mName));

	return res;
}

void EditorApp::installEventFilter(QObject* pObject)
{
	QApplication::installEventFilter(pObject);
}

void EditorApp::removeEventFilter(QObject* pObject)
{
	QApplication::removeEventFilter(pObject);
}