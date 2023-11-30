#include "Editor/Widget/WidgetSelectable.h"
#include <QtWidgets/qstyle.h>


using namespace Editor::Widget;

WidgetSelectable::WidgetSelectable(QSettings& pSetting, const QString& pProjectName, const QString& pIcon, bool pIsAnimation, QWidget* pParent) : QWidget(pParent)
{
	setObjectName("ProjectSelectionWidget");
	setFixedSize(pSetting.value("ProjectSelectionWidgetWidth").toInt(), pSetting.value("ProjectSelectionWidgetHeight").toInt());
	setProperty("pressed", false);

	setLayout(&mLayout);


	mImage.setFixedSize(pSetting.value("ProjectSelectionImageWidth").toInt(), pSetting.value("ProjectSelectionImageHeight").toInt());
	mImage.setScaledContents(true);
	mImage.setObjectName("ProjectWidget");
	mLayout.addWidget(&mImage);
	
	if (!pIsAnimation)
	{
		mImage.setPixmap(QPixmap(pIcon));
	}
	else
	{
		if (pIcon.contains("gif"))
		{
			mMovie = new QMovie(pIcon);
			mImage.setMovie(mMovie);
			mMovie->start();
		}
		else
			mImage.setPixmap(QPixmap(pIcon));
	}
	
	mText.setAlignment(Qt::AlignHCenter);
	mText.setReadOnly(true);
	mText.setText(pProjectName);
	mText.setObjectName("ProjectWidgetLabel");
	mLayout.addWidget(&mText);

	mLayout.addStretch(mLayout.count());
}

WidgetSelectable::~WidgetSelectable()
{
	if (mMovie != nullptr)
		delete mMovie;
}

void WidgetSelectable::setClicked(const bool& pChecked)
{
	if (pChecked == mChecked)
		return;

	mChecked = pChecked;
	setProperty("pressed", mChecked);
	style()->unpolish(this);
	style()->polish(this);
}