#pragma once
#include <QtCore/qstring.h>

namespace Editor::Resources::Loaders
{
	class StyleSheetLoader
	{
		public:
			StyleSheetLoader() = delete;

			static QString loadStyleSheet(const QString& pPath);
	};
}