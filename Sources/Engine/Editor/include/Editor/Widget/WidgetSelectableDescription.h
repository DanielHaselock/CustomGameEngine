#pragma once
#include "Editor/Widget/WidgetSelectable.h"
#include "Editor/Widget/WidgetTemplateDescription.h"

namespace Editor::Widget
{
	class WidgetSelectableDescription : public WidgetSelectable
	{
		public:
			WidgetTemplateDescription* mRightWindow = nullptr;

			WidgetSelectableDescription(QSettings& pSetting, const QString& pProjectName, const QString& pIcon, const QString& pGroupTag, QHBoxLayout& pVLayout, QWidget* pParent = nullptr);
			virtual void setClicked(const bool& pChecked) override;
	};
}