#pragma once
#include <QtCore/qstring.h>

namespace Editor::Data
{
	class NewDocument
	{
		public:
			bool mIsNew = false;
			QString mPath = "";

			NewDocument(QString pPath = "", bool pIsNew = false);
	};
}