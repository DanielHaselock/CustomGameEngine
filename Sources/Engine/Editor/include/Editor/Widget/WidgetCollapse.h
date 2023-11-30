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
    class WidgetCollapse : public QWidget
    {
        public:
            QGridLayout mainLayout;
            QToolButton toggleButton;
            QParallelAnimationGroup toggleAnimation;
            QScrollArea contentArea;
            int animationDuration{ 300 };
            QSettings& mSetting;
            Game::Data::Actor& mActor;
            Game::Component::AComponent& mComponent;
            Game::Utils::ComponentType mType;

            WidgetCollapse(QSettings& pSetting ,Game::Data::Actor& pActor, Game::Component::AComponent& pComponent, Game::Utils::ComponentType pType, const QIcon& pIcon,
                const QString& title = "", bool hasAnimation = true, const int animationDuration = 100, QWidget* parent = 0);
            void setContentLayout(QLayout& contentLayout);

            void showMenu(const QPoint& pPos);


    };
}