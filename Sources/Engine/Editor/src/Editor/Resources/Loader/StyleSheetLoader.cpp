#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include <QtCore/qfile.h>

using namespace Editor::Resources::Loaders;

QString StyleSheetLoader::loadStyleSheet(const QString& pPath)
{
	QFile styleSheetFile(pPath);
	styleSheetFile.open(QFile::ReadOnly);
	return QLatin1String(styleSheetFile.readAll());
}