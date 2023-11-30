#pragma once
#include <QtWidgets/qapplication.h>
#include <tuple>
#include <string>

#include "Tools/Utils/Define.h"

namespace Editor::Core
{
	class EditorApp : QApplication
	{
	public:
		EditorApp(int& pArgc);
		std::tuple<bool, std::string, std::string> EditorApp::runHub();
		Define::EEditorState runEditor(const std::string& pProjectPath, const std::string& projectName);
		void installEventFilter(QObject* pObject);
		void removeEventFilter(QObject* pObject);
	};
}