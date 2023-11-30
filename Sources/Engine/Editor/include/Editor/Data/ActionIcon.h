#pragma once
#include <QtGui/qaction.h>
#include <QtGui/qicon.h>
#include <QtCore/qstring.h>

namespace Editor::Data
{

	class ActionIcon
	{
		public:
			QIcon mIcon;
			QIcon mIconActif;
			QAction* mAction = nullptr;

			ActionIcon();
			void init(const QString& pIcon, const QString& pIconActif, QAction* pAction);
			void setTips(const QString& pTips);
			void setDisabled(bool pState);
			void switchIcon(const QIcon& pIcon);
			void switchIcon(const int idx);
	};
}