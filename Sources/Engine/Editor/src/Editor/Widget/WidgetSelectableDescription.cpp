#include "Editor/Widget/WidgetSelectableDescription.h"

using namespace Editor::Widget;

WidgetSelectableDescription::WidgetSelectableDescription(QSettings& pSetting, const QString& pProjectName, const QString& pIcon, const QString& pGroupTag, QHBoxLayout& pVLayout, QWidget* pParent) :
	WidgetSelectable(pSetting, pProjectName, pIcon, pParent)
{
	mRightWindow = new WidgetTemplateDescription(pSetting, pGroupTag);
	mRightWindow->setVisible(false);
	pVLayout.addWidget(mRightWindow);
}

void WidgetSelectableDescription::setClicked(const bool& pChecked)
{
	if (pChecked == mChecked)
		return;

	mRightWindow->setVisible(pChecked);
	WidgetSelectable::setClicked(pChecked);
}