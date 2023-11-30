#include "Editor/Widget/WidgetMeshMatDialog.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qlineedit.h>
#include "Editor/Utils/Utils.h"
#include "QtWidgets/qpushbutton.h"
#include <QtWidgets/qscrollarea.h>
#include <Editor/Widget/WidgetDropInput.h>
#include <Editor/Widget/WidgetDropInputMat.h>
#include "EngineCore/Service/ServiceLocator.h"
#include "Editor/Data/ProjectLocation.h"

using namespace Editor::Widget;

WidgetMeshMatDialog::WidgetMeshMatDialog(Game::Component::CPModel* pModel, QWidget* pParent) :
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

void WidgetMeshMatDialog::setWindowDecoration()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(mSettings.value("MainWindowWidth").toInt(), mSettings.value("MainWindowHeight").toInt());
}

void WidgetMeshMatDialog::initWindowCloseButton()
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
    connect(quitAtion, &QAction::triggered, this, &WidgetMeshMatDialog::close);
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

void WidgetMeshMatDialog::initCentralWidget()
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


        for (auto& mesh : mModel->mModel->mMeshes)
        {
            if (mMats.find(mesh->mName) != mMats.end())
            {
                mMats[mesh->mName].push_back(mesh);
                continue;
            }

            mMats[mesh->mName].push_back(mesh);

            QWidget* dataContainer = new QWidget();

            QVBoxLayout* boxContainer = new QVBoxLayout();
            dataContainer->setLayout(boxContainer);


            QLabel* label = new QLabel();
            label->setText(mesh->mName.c_str());
            label->setMinimumHeight(25);
            label->setMaximumHeight(25);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            label->setObjectName("GameObject");
            
            boxContainer->addWidget(label, 0, Qt::AlignTop | Qt::AlignHCenter);


            WidgetDropInputMat* mMatPath = new WidgetDropInputMat(*mModel);
            if (mModel->mMeshMat.find(mesh) != mModel->mMeshMat.end())
                mMatPath->setText(mModel->mMeshMat[mesh]->mFilename.c_str());
            mMatPath->setReadOnly(true);
            boxContainer->addWidget(mMatPath);
            connect(mMatPath, &WidgetDropInput::textChanged, this, [this, mMatPath, mesh]
            {
                for (Rendering::Resources::VK::VKMesh* otherMesh : mMats[mesh->mName])
                    mModel->mMeshMat[otherMesh] = nullptr;
            });

            const QIcon folderIcon(mSettings.value("FolderIcon").toString());
            QAction* browseAction = new QAction(folderIcon, mSettings.value("FolderTip").toString());
            mMatPath->addAction(browseAction, QLineEdit::TrailingPosition);
            connect(browseAction, &QAction::triggered, this, [this, mMatPath, mesh]
            {
                QString Defaultpath = service(Editor::Data::ProjectLocation).mFolder;

                std::string filter = "Mat Files (*.mat)";
                QFileDialog dialog(this, tr("Open Material"), Defaultpath, tr(filter.c_str()));
                connect(&dialog, &QFileDialog::directoryEntered, [&dialog, Defaultpath, this](QString path)
                {
                    if (!path.contains(Defaultpath))
                        dialog.setDirectory(Defaultpath);
                });

                if (dialog.exec() != QDialog::Accepted)
                    return;

                QString fileName = dialog.selectedFiles().first();
                if (!fileName.isEmpty())
                {
                    QFileInfo info(fileName);

                    mMatPath->setText(info.absoluteFilePath());

                    std::string str = Editor::Utils::qStringToStdString(info.absoluteFilePath());
                    for (Rendering::Resources::VK::VKMesh* otherMesh : mMats[mesh->mName])
                        mModel->mMeshMat[otherMesh] = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>(str.c_str(), str)->getInstance();
                }
            });


            scrollVBox->addWidget(dataContainer, 0, Qt::AlignTop);
        }

        scrollVBox->addStretch();

        scroll->setWidget(scrollCentral);
        scroll->adjustSize();

        vBox->addWidget(scroll, 0, Qt::AlignVCenter);
    }
}

void WidgetMeshMatDialog::initBottom()
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
    connect(accept, &QAction::triggered, this, &WidgetMeshMatDialog::quit);
}

bool WidgetMeshMatDialog::eventFilter(QObject* obj, QEvent* event)
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

void WidgetMeshMatDialog::quit()
{
    close();
    setResult(0);
}