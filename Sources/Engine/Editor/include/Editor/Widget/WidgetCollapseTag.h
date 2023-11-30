#include <QtWidgets/qframe.h>
#include <QtWidgets/QGridLayout>
#include <QtCore/QParallelAnimationGroup>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include "Game/Data/Actor.h"
#include "Game/Component/AComponent.h"
#include "Game/Utils/ComponentType.h"
#include <QtCore/qsettings.h>
namespace Editor::Widget
{
    class WidgetCollapseTag : public QWidget
    {
        public:
            QGridLayout mainLayout;
            QToolButton toggleButton;
            QParallelAnimationGroup toggleAnimation;
            QScrollArea contentArea;
            int animationDuration{ 300 };
            QSettings& mSetting;
            Game::Data::Actor& mActor;
            

            WidgetCollapseTag(QSettings& pSetting, Game::Data::Actor& pActor, const QIcon& pIcon,
                const QString& title = "", const int animationDuration = 100, QWidget* parent = 0);
            void setContentLayout(QLayout& contentLayout);

            void showMenu(const QPoint& pPos);


    };
}