#include "Editor/Widget/WidgetSocketDialog.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qlineedit.h>
#include "Editor/Utils/Utils.h"
#include "QtWidgets/qpushbutton.h"
#include <QtWidgets/qscrollarea.h>

using namespace Editor::Widget;

WidgetSocketDialog::WidgetSocketDialog(Game::Component::CPModel* pModel, QWidget* pParent) :
    QDialog(pParent),
    mModel(pModel),
    mSettings("Resources/Editor/Config/StyleUIEN.ini", QSettings::IniFormat)
{
    setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));

    setWindowDecoration();
    initWindowCloseButton();
    initCentralWidget();
    initBottom();
}

void WidgetSocketDialog::setWindowDecoration()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(mSettings.value("MainWindowWidth").toInt(), mSettings.value("MainWindowHeight").toInt());
}

void WidgetSocketDialog::initWindowCloseButton()
{
    mWindow = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mWindow->setSpacing(mSettings.value("MainWindowSpacing").toInt());
    mWindow->setContentsMargins(
        mSettings.value("MainWindowMarginLeft").toInt(),
        mSettings.value("MainWindowMarginTop").toInt(),
        mSettings.value("MainWindowMarginRight").toInt(),
        mSettings.value("MainWindowMarginBottom").toInt());

    QToolBar* toolBar = new QToolBar("windowManager");
    toolBar->setObjectName("TitleBar");
    toolBar->setLayoutDirection(Qt::RightToLeft);
    mWindow->addWidget(toolBar);

    const QIcon quitIcon(mSettings.value("CloseIcon").toString());
    QAction* quitAtion = new QAction(quitIcon, mSettings.value("CloseTip").toString());
    quitAtion->setShortcuts(QKeySequence::Quit);
    connect(quitAtion, &QAction::triggered, this, &WidgetSocketDialog::close);
    toolBar->addAction(quitAtion);


    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(spacer);


    QLabel* title = new QLabel(mSettings.value("Title").toString());
    title->setAlignment(Qt::AlignVCenter);
    toolBar->addWidget(title);


    QWidget* spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(spacer2);


    QLabel* icon = new QLabel();
    icon->setPixmap(QPixmap(mSettings.value("Icon").toString()).scaled(mSettings.value("IconWidth").toInt(), mSettings.value("IconHeight").toInt()));
    icon->setContentsMargins(
        mSettings.value("IconMarginLeft").toInt(),
        mSettings.value("IconMarginUp").toInt(),
        mSettings.value("IconMarginRight").toInt(),
        mSettings.value("IconMarginBottom").toInt());
    icon->setObjectName("IconTitle");
    icon->setAlignment(Qt::AlignHCenter);
    toolBar->addWidget(icon);
}

void WidgetSocketDialog::initCentralWidget()
{
    central = new QWidget();
    mWindow->addWidget(central);
    mWindow->setObjectName("central");
    
    QVBoxLayout* vBox = new QVBoxLayout();
    central->setLayout(vBox);

    if (mModel->mModel == nullptr)
        return;

    {
        QWidget* scrollCentral = new QWidget();

        QVBoxLayout* scrollVBox = new QVBoxLayout();
        scrollCentral->setLayout(scrollVBox);

        QScrollArea* scroll = new QScrollArea(central);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scroll->setWidgetResizable(true);
        scroll->setGeometry(0, 0, mSettings.value("MainWindowWidth").toInt(), 800);

        QLabel* label = new QLabel();
        label->setText(mModel->mName.c_str());
        label->setMinimumHeight(25);
        label->setMaximumHeight(25);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        label->setObjectName("GameObject");
        scrollVBox->addWidget(label, 0, Qt::AlignTop | Qt::AlignHCenter);

        for (auto& it : mModel->mModel->mBoneInfoMap)
        {
            QLineEdit* tag = new QLineEdit();
            tag->setText(it.first.c_str());
            tag->setMinimumHeight(25);
            tag->setMaximumHeight(25);
            tag->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            tag->setObjectName("GameObject");
            tag->setReadOnly(true);

            scrollVBox->addWidget(tag, 0, Qt::AlignTop);
        }

        scrollVBox->addStretch();

        scroll->setWidget(scrollCentral);
        scroll->adjustSize();

        vBox->addWidget(scroll, 0, Qt::AlignVCenter);
    }

    //vBox->addStretch();
}

void WidgetSocketDialog::initBottom()
{
    QWidget* mBottomWindow = new QWidget();
    mBottomWindow->setObjectName("Bottom");
    mBottomWindow->setFixedHeight(mSettings.value("BottomWindowHeight").toInt());
    mWindow->addWidget(mBottomWindow);

    QHBoxLayout* mBottomVerticaltalLayout = new QHBoxLayout();
    mBottomWindow->setLayout(mBottomVerticaltalLayout);

    QToolBar* mBottomToolBar = new QToolBar("ProjectButton");
    mBottomVerticaltalLayout->addWidget(mBottomToolBar);
    mBottomToolBar->setLayoutDirection(Qt::RightToLeft);
    mBottomToolBar->setOrientation(Qt::Horizontal);
    mBottomToolBar->setObjectName("ProjectButtons");
    mBottomToolBar->setContentsMargins(QMargins(
        mSettings.value("BottomToolBarMarginLeft").toInt(),
        mSettings.value("BottomToolBarMarginTop").toInt(),
        mSettings.value("BottomToolBarMarginRight").toInt(),
        mSettings.value("BottomToolBarMarginBottom").toInt()));


    QAction* accept = new QAction(mSettings.value("BottomToolBarAccept").toString());
    mBottomToolBar->addAction(accept);
    mBottomToolBar->widgetForAction(accept)->setObjectName("Blue");
    connect(accept, &QAction::triggered, this, &WidgetSocketDialog::quit);
}

bool WidgetSocketDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        
    }
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = (QKeyEvent*)event;
        if (keyEvent->key() == Qt::Key_Escape)
            quit();
    }

    return QObject::eventFilter(obj, event);
}

void WidgetSocketDialog::quit()
{
    close();
    setResult(0);
}