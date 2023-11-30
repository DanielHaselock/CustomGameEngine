#pragma once
#include <QtCore/qstring.h>
#include <string>
#include <QtWidgets/qwidget.h>
#include <QtGui/qevent.h>
#include <EngineCore/EventSystem/Key.h>
#include <Tools/Utils/PathParser.h>

namespace Editor
{
	class Utils
	{
		public:
			Utils() = delete;

			static QString uniqueString(const QString& pBase, const char* pExtension = nullptr);
			static void copyAndReplaceFolderContents(const QString& pFromDir, const QString& pToDir, bool pCopyAndRemove = false, QString pExeptFile = "");
			static void copyFileAndDir(const QString& pFromDir, const QString& pToDir, QWidget* pParent, bool pCopyAndRemove = false);
			static std::string qStringToStdString(const QString& pString);

			static void sendQtInput(QKeyEvent* keyEvent, EngineCore::EventSystem::KState pState);

			static Tools::Utils::PathParser::EFileType getFileType(const QString& pExtension);
	};
}