#include "Editor/Widget/WidgetSceneLabel.h"

using namespace Editor::Widget;

WidgetSceneLabel::WidgetSceneLabel()
{
}

void WidgetSceneLabel::init(QWidget* pParent, QSettings& pSetting, int pHeight, const QString& pScenePath, const QString& pSceneName)
{
    mWidget = new QWidget(pParent);
    mWidget->move(pSetting.value("IconWidth").toInt() + 300, pHeight);
    mWidget->setObjectName("LevelName");
    mWidget->setFixedWidth(pSetting.value("LevelNameWidget").toInt());

    QHBoxLayout* hLayout = new QHBoxLayout();
    mWidget->setLayout(hLayout);

    QLabel* icon = new QLabel();
    icon->setObjectName("LevelNameIcon");
    icon->setPixmap(QPixmap(pSetting.value("LevelIcon").toString()).scaled(pSetting.value("LevelIconWidth").toInt(), pSetting.value("LevelIconHeight").toInt()));
    hLayout->addWidget(icon);

    mMapTitle = new QLabel(pSceneName);
    mMapTitle->setObjectName("LevelName");
    hLayout->addWidget(mMapTitle);
    hLayout->addStretch(1);

    mFileName = pScenePath;
}