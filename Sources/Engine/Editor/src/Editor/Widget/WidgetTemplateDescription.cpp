#include "Editor/Widget/WidgetTemplateDescription.h"


using namespace Editor::Widget;

WidgetTemplateDescription::WidgetTemplateDescription(QSettings& pSetting, const QString& pGroupTag, QWidget* pParent)
{
	setObjectName("TemplateDescription");
	setFixedWidth(pSetting.value("TemplateDescriptionWidth").toInt());

	setLayout(&mLayout);

	mImage.setFixedHeight(pSetting.value("TemplateImageHeight").toInt());
	mImage.setScaledContents(true);

	pSetting.beginGroup(pGroupTag);
	QPixmap QImage = QPixmap(pSetting.value("Image").toString());
	mImage.setPixmap(QImage);
	mImage.setObjectName("TemplateDescriptionImage");
	mLayout.addWidget(&mImage, Qt::AlignTop);

	mTitle.setText(pSetting.value("Title").toString());
	mTitle.setObjectName("ProjectWidgetLabelTitle");
	mLayout.addWidget(&mTitle);

	mText.setText(pSetting.value("Description").toString());
	mText.setObjectName("ProjectWidgetLabelDescription");
	mLayout.addWidget(&mText);
	pSetting.endGroup();

	mLayout.addStretch(mLayout.count());
}