#include <QtCore/qdir.h>
#include "Editor/Widget/WidgetDropUI.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtoolbar.h>
#include "Editor/Widget/WidgetStyleSkyBox.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include "Tools/Utils/Define.h"
#include "Editor/Utils/Utils.h"
#include <Editor/Widget/WidgetCollapse.h>
#include <QtWidgets/qcheckbox.h>
#include <Editor/Widget/WidgetCollapseUI.h>
#include "Editor/Widget/WidgetDropInput.h"
#include <QtWidgets/qcolordialog.h>
#include "Editor/Data/ProjectLocation.h"

using namespace Editor::Widget;

WidgetStyleSkyBox::WidgetStyleSkyBox(Game::Component::CPCamera& pCamera, QWidget* pParent) :
    QDialog(pParent),
    mCamera(pCamera),
    mSettings("Resources/Editor/Config/StyleUIEN.ini", QSettings::IniFormat)
{
    setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));

    setWindowDecoration();
    initWindowCloseButton();
    initCentralWidget();
    initBottom();
}

void WidgetStyleSkyBox::setWindowDecoration()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(mSettings.value("MainWindowWidth").toInt(), mSettings.value("MainWindowHeight").toInt());
}

void WidgetStyleSkyBox::initWindowCloseButton()
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
    connect(quitAtion, &QAction::triggered, this, &WidgetStyleSkyBox::close);
    toolBar->addAction(quitAtion);


    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(spacer);


    QLabel* title = new QLabel("SkyBox texture");
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

void WidgetStyleSkyBox::initCentralWidget()
{
    QWidget* central = new QWidget();
    mWindow->addWidget(central);
    mWindow->setObjectName("central");
    
    QVBoxLayout* vBox = new QVBoxLayout();
    central->setLayout(vBox);

    // Left
    setTextureDialogue("Left Texture", 0, *vBox);

    // Right
    setTextureDialogue("Right Texture", 1, *vBox);

    // Top
    setTextureDialogue("Top Texture", 2, *vBox);

    // Botton
    setTextureDialogue("Botton Texture", 3, *vBox);

    // Front
    setTextureDialogue("Front Texture", 4, *vBox);

    // Back
    setTextureDialogue("Back Texture", 5, *vBox);

}

void WidgetStyleSkyBox::initBottom()
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
    connect(cancel, &QAction::triggered, this, &WidgetStyleSkyBox::quit);

    QAction* accept = new QAction(mSettings.value("BottomToolBarAccept").toString());
    mBottomToolBar->addAction(accept);
    mBottomToolBar->widgetForAction(accept)->setObjectName("Blue");
    connect(accept, &QAction::triggered, this, &WidgetStyleSkyBox::quit);
}

bool WidgetStyleSkyBox::eventFilter(QObject* obj, QEvent* event)
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

void WidgetStyleSkyBox::quit()
{
    close();
    setResult(0);
}


void WidgetStyleSkyBox::setTextureDialogue(QString name, int idx, QVBoxLayout& box)
{
    int textureCount = mCamera.skybox.paths.size();

    QWidget* wiz = new QWidget();

    QGridLayout* widLayout = new QGridLayout();
    widLayout->setVerticalSpacing(2);

    wiz->setLayout(widLayout);

    // Script name
    widLayout->addWidget(new QLabel(name));

    // Script path
    QLineEdit* mPath = new QLineEdit();
    mPath->setText(textureCount > idx ? QString(mCamera.skybox.paths[idx].c_str()) : "");
    mPath->setReadOnly(true);
    widLayout->addWidget(mPath);


    const QIcon folderIcon(mSettings.value("FolderIcon").toString());
    QAction* browseAction = new QAction(folderIcon, mSettings.value("FolderTip").toString());
    mPath->addAction(browseAction, QLineEdit::TrailingPosition);
    connect(browseAction, &QAction::triggered, this, [=]
    {
        QString Defaultpath = service(Data::ProjectLocation).mFolder;

        std::string filter = "Texture File (*.png *.jpg *.jpeg)";

        QFileDialog dialog(this, tr("Open Texture"), Defaultpath, tr(filter.c_str()));
        connect(&dialog, &QFileDialog::directoryEntered, [&dialog, Defaultpath, this](QString path)
            {
                if (!path.contains(Defaultpath))
                    dialog.setDirectory(Defaultpath);
            });

        if (dialog.exec() != QDialog::Accepted)
            return;

        QString fileName = dialog.selectedFiles().first();
        if (!fileName.isEmpty())
            mPath->setText(fileName);
    });

    box.insertWidget(box.count(), wiz, 0, Qt::AlignTop);

    lineEdit.push_back(mPath);
}