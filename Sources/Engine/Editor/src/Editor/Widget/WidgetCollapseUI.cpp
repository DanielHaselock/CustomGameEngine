#include "Editor/Widget/WidgetCollapseUI.h"
#include <QtCore/qpropertyanimation.h>
#include <QtWidgets/qmenu.h>
using namespace Editor::Widget;

WidgetCollapseUI::WidgetCollapseUI(QSettings& pSetting, const QIcon& pIcon, const QString& title, const int animationDuration, QWidget* parent)
    : QWidget(parent), animationDuration(animationDuration), mSetting(pSetting)
{
    toggleButton.setStyleSheet("QToolButton { color:#d2d2d2; background-color: #3e3e3e; border: 0px; }");
    toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //toggleButton.setArrowType(Qt::ArrowType::RightArrow);
    toggleButton.setIcon(pIcon);
    toggleButton.setText(title);
    toggleButton.setCheckable(true);
    toggleButton.setChecked(false);
    toggleButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);


    contentArea.setStyleSheet("QScrollArea { background-color: #242424; border: none; }");
    contentArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // start out collapsed
    contentArea.setMaximumHeight(0);
    contentArea.setMinimumHeight(0);
    // let the entire widget grow and shrink with its content
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));
    // don't waste space
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0, 0, 0, 0);

    mainLayout.addWidget(&toggleButton, 0, 0);
    mainLayout.addWidget(&contentArea, 1, 0);
    setLayout(&mainLayout);
    QObject::connect(&toggleButton, &QToolButton::clicked, [this, &pIcon](const bool checked)
    {
        //toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
        toggleAnimation.setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        toggleAnimation.start();
    });


    setContextMenuPolicy(Qt::CustomContextMenu);
}

void WidgetCollapseUI::setContentLayout(QLayout& contentLayout)
{
    delete contentArea.layout();
    contentArea.setLayout(&contentLayout);
    const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
    auto contentHeight = contentLayout.sizeHint().height();
    for (int i = 0; i < toggleAnimation.animationCount() - 1; ++i)
    {
        QPropertyAnimation* spoilerAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(i));
        spoilerAnimation->setDuration(animationDuration);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }
    QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}