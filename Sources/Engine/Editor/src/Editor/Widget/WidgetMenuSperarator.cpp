#include "Editor/Widget/WidgetMenuSperarator.h"

using namespace Editor::Widget;

WidgetMenuSeperator::WidgetMenuSeperator(const QSettings& pSetting, const QString& pName, const QString& pIcon, QWidget* pWidget)
{
    setObjectName("Separator");
    setFixedHeight(pSetting.value("SeparatorWidgetHeight").toInt());

    mHLayout = new QHBoxLayout();
    mHLayout->setContentsMargins(pSetting.value("SeparatorWidgetMaringLeft").toInt(),
                                                pSetting.value("SeparatorWidgetMaringTop").toInt(),
                                                pSetting.value("SeparatorWidgetMaringRight").toInt(),
                                                pSetting.value("SeparatorWidgetMaringBottom").toInt());
    setLayout(mHLayout);

    mapTitle = new QLabel(pName);
    mapTitle->setObjectName("Separator");
    mHLayout->addWidget(mapTitle);

    icon = new QLabel();
    mHLayout->addWidget(icon, 1);
    icon->setObjectName("SeparatorIcon");
    icon->setScaledContents(true);
    icon->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    icon->setPixmap(QPixmap(pIcon).scaledToHeight(pSetting.value("SeparatorIconHeight").toInt()));

    mAction = new QWidgetAction(pWidget);
    mAction->setDefaultWidget(this);
}