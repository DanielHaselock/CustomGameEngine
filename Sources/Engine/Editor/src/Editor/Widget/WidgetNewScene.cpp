#include <QtCore/qdir.h>

#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtoolbar.h>
#include "Editor/Widget/WidgetNewScene.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include "Tools/Utils/Define.h"
#include "Editor/Utils/Utils.h"

using namespace Editor::Widget;

WidgetNewScene::WidgetNewScene(const QString& pProjectDir, QWidget* pParent) :
    QDialog(pParent),
    mProjectDir(pProjectDir),
    mSettings("Resources/Editor/Config/NewLevelEN.ini", QSettings::IniFormat)
{
    setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));

    setWindowDecoration();
    initWindowCloseButton();
    initCentralWidget();
    initBottom();
}

void WidgetNewScene::setWindowDecoration()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(mSettings.value("MainWindowWidth").toInt(), mSettings.value("MainWindowHeight").toInt());
}

void WidgetNewScene::initWindowCloseButton()
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
    connect(quitAtion, &QAction::triggered, this, &WidgetNewScene::close);
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

void WidgetNewScene::initCentralWidget()
{
    QWidget* central = new QWidget();
    mWindow->addWidget(central);
    mWindow->setObjectName("central");

    QGridLayout* mGrid = new QGridLayout();
    mGrid->setContentsMargins(
        mSettings.value("RightGridMarginLeft").toInt(),
        mSettings.value("RightGridMarginTop").toInt(),
        mSettings.value("RightGridMarginRight").toInt(),
        mSettings.value("RightGridMarginBottom").toInt());
    central->setLayout(mGrid);

    //View one
    {
        WidgetSelectable* proj = new WidgetSelectable(mSettings, "Open World", mSettings.value("LevelOpenMap").toString());
        proj->setClicked(true);
        mCurrent = proj;
        installEventFilter(proj);
        mGrid->addWidget(proj, 0, 0, Qt::AlignLeft | Qt::AlignTop);
        mChoice.push_back(proj);
    }

    {
        WidgetSelectable* proj = new WidgetSelectable(mSettings, "Empty Open World", mSettings.value("LevelEmptyOpenMap").toString());
        installEventFilter(proj);
        mGrid->addWidget(proj, 0, 1, Qt::AlignLeft | Qt::AlignTop);
        mChoice.push_back(proj);
    }

    {
        WidgetSelectable* proj = new WidgetSelectable(mSettings, "Basic", mSettings.value("LevelBasic").toString());
        installEventFilter(proj);
        mGrid->addWidget(proj, 0, 2, Qt::AlignLeft | Qt::AlignTop);
        mChoice.push_back(proj);
    }

    {
        WidgetSelectable* proj = new WidgetSelectable(mSettings, "Empty Level", mSettings.value("LevelEmptyLevel").toString());
        installEventFilter(proj);
        mGrid->addWidget(proj, 0, 3, Qt::AlignLeft | Qt::AlignTop);
        mChoice.push_back(proj);
    }

    mGrid->setRowStretch(mGrid->rowCount(), mSettings.value("RightGridStretch").toInt());
    mGrid->setColumnStretch(mGrid->columnCount(), mSettings.value("RightGridStretch").toInt());
}

void WidgetNewScene::initBottom()
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


    QAction* cancel = new QAction(mSettings.value("BottomToolBarCancel").toString());
    mBottomToolBar->addAction(cancel);
    connect(cancel, &QAction::triggered, this, &WidgetNewScene::quit);

    QAction* create = new QAction(mSettings.value("BottomToolBarCreate").toString());
    mBottomToolBar->addAction(create);
    mBottomToolBar->widgetForAction(create)->setObjectName("Blue");
    connect(create, &QAction::triggered, this, &WidgetNewScene::createLevel);
}

bool WidgetNewScene::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (obj->objectName() == "ProjectSelectionWidget" || obj->objectName() == "ProjectWidgetLabel")
        {
            for (unsigned int i = 0; i < mChoice.size(); i++)
            {
                if (obj == mChoice[i] || obj == &mChoice[i]->mText)
                {
                    mChoice[i]->setClicked(true);
                    mCurrent = mChoice[i];
                }
                else
                    mChoice[i]->setClicked(false);
            }
        }
    }
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = (QKeyEvent*)event;
        if (keyEvent->key() == Qt::Key_Escape)
            quit();
    }

    return QObject::eventFilter(obj, event);
}

void WidgetNewScene::createLevel()
{
    if (!QDir(mProjectDir + "/Map").exists())
        QDir().mkdir(mProjectDir + "/Map");

    mNewScene = Utils::uniqueString(mProjectDir + "/Map/" + mSettings.value(mCurrent->mText.text()).toString().split("/").last(), Define::SCENE_EXTENSION);
    QString sdtr = QDir::currentPath() + "/" + mSettings.value(mCurrent->mText.text()).toString() + "." + Define::SCENE_EXTENSION;
    QFile::copy(sdtr, mNewScene);

    close();
    setResult(1);
}

void WidgetNewScene::quit()
{
    close();
    setResult(0);
}