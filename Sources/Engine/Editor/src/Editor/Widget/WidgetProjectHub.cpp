#include "Editor/Widget/WidgetProjectHub.h"

#include <QtCore/qstandardpaths.h>
#include <QtCore/qdir.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qstyle.h>
#include <QtGui/qscreen.h>
#include <QtWidgets/qformlayout.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>

#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include "Editor/Utils/Utils.h"
#include "Tools/Utils/Define.h"

using namespace Editor::Widget;

Editor::Widget::RightView::RightView(const QString& pName, const QMargins& margin)
{
    setObjectName(pName);
    mGrid->setContentsMargins(margin.left(), margin.top(), margin.right(), margin.bottom());
}

WidgetProjectHub::WidgetProjectHub() : 
    QWidget(nullptr), 
    mSettings("Resources/Editor/Config/LauncherEN.ini", QSettings::IniFormat)
{
    setTheme();
    setStandardProjectLocation();
    setWindowDecoration();
    registerMetaType();
    readSavedData();

    initWindowControlButtons();
    initCenter();
    initLeftCenter();
    initRightSide();
    initBottomForm();

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QApplication::primaryScreen()->availableGeometry()));

    if (mRecentProjects.size() == 0)
    {
        updateReadOnlyField(mNewProject, false);
        leftViewSelected(mLeftActions[1]);
    }

    show();
    setFocus();
}

void Editor::Widget::WidgetProjectHub::setTheme()
{
    setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));
}

void WidgetProjectHub::setStandardProjectLocation()
{
    QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + mSettings.value("DefaultLocation").toString();
    QDir pathDir(location);
    if (!pathDir.exists(location))
        pathDir.mkdir(".");

    mNewProject = Data::ProjectLocation(location, mSettings.value("DefaultProjectName").toString());
}

void WidgetProjectHub::setWindowDecoration()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setFixedSize(mSettings.value("WindowWidth").toInt(), mSettings.value("WindowHeight").toInt());
}

void WidgetProjectHub::registerMetaType()
{
    qRegisterMetaType<Data::ProjectLocation>();
    qRegisterMetaType<QVector<Data::ProjectLocation>>();
}

void WidgetProjectHub::readSavedData()
{
    QSettings settings(QApplication::applicationDirPath() + "/" + mSettings.value("LauncherSaveFile").toString(), QSettings::IniFormat, Q_NULLPTR);
    QVector<Data::ProjectLocation> tmp = settings.value("RecentProjects").value<QVector<Data::ProjectLocation>>();

    for (unsigned int i = 0; i < tmp.size(); i++)
    {
        QString str = tmp[i].mFolder + "/" + tmp[i].mName + "." + Define::PROJECT_EXTENSION;
        if (QFile::exists(str))
            mRecentProjects.push_back(tmp[i]);
    }
}

void WidgetProjectHub::writeSaveData()
{
    QVector<Data::ProjectLocation> newOrder;
    newOrder.push_back(*mCurrentProject);

    for (unsigned int i = 0; i < mRecentProjects.size(); i++)
    {
        if (mRecentProjects[i] == *mCurrentProject)
            continue;

        newOrder.push_back(mRecentProjects[i]);
    }

    QSettings settings(QApplication::applicationDirPath() + "/" + mSettings.value("LauncherSaveFile").toString(), QSettings::IniFormat, Q_NULLPTR);
    settings.setValue("RecentProjects", QVariant::fromValue(newOrder));
}

void WidgetProjectHub::initWindowControlButtons()
{
    //create window container
    mWindow = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mWindow->setSpacing(mSettings.value("MainWindowSpacing").toInt());
    mWindow->setContentsMargins(
        mSettings.value("WindowMarginLeft").toInt(),
        mSettings.value("WindowMarginTop").toInt(),
        mSettings.value("WindowMarginRight").toInt(),
        mSettings.value("WindowMarginBottom").toInt());

    // Define button
    QToolBar* windowButtons = new QToolBar("windowManager");
    windowButtons->setObjectName("TitleBar");
    windowButtons->setLayoutDirection(Qt::RightToLeft);
    
    const QIcon quitIcon(mSettings.value("CloseIcon").toString());
    QAction* quitAtion = new QAction(quitIcon, mSettings.value("CloseTip").toString());
    quitAtion->setShortcuts(QKeySequence::Quit);
    connect(quitAtion, &QAction::triggered, this, &WidgetProjectHub::close);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    QLabel* title = new QLabel(mSettings.value("Title").toString());
    title->setAlignment(Qt::AlignVCenter);
    
    QWidget* spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    QLabel* icon = new QLabel();
    icon->setPixmap(QPixmap(mSettings.value("Icon").toString()).scaled(mSettings.value("IconWidth").toInt(), mSettings.value("IconHeight").toInt()));
    icon->setContentsMargins(
        mSettings.value("IconMarginLeft").toInt(),
        mSettings.value("IconMarginUp").toInt(),
        mSettings.value("IconMarginRight").toInt(),
        mSettings.value("IconMarginBottom").toInt());
    icon->setObjectName("IconTitle");
    icon->setAlignment(Qt::AlignHCenter);
    
    //Adding button to window
    mWindow->addWidget(windowButtons);
    windowButtons->addAction(quitAtion);
    windowButtons->addWidget(spacer);
    windowButtons->addWidget(title);
    windowButtons->addWidget(spacer2);
    windowButtons->addWidget(icon);
}

void WidgetProjectHub::initCenter()
{
    mCenter = new QHBoxLayout();
    mWindow->addLayout(mCenter);
}

void WidgetProjectHub::initLeftCenter()
{
    //Create left container
    QVBoxLayout* leftSide = new QVBoxLayout();
    leftSide->setContentsMargins(
        mSettings.value("LeftSideMarginLeft").toInt(),
        mSettings.value("LeftSideMarginTop").toInt(),
        mSettings.value("LeftSideMarginRight").toInt(),
        mSettings.value("LeftSideMarginBottom").toInt());

    QWidget* leftWindow = new QWidget();
    leftWindow->setObjectName("Left");
    
    QToolBar* leftButtonContainer = new QToolBar(leftWindow);
    leftButtonContainer->setObjectName("LeftButtons");
    leftButtonContainer->setOrientation(Qt::Vertical);
    leftButtonContainer->setFixedSize(mSettings.value("LeftSideButtonWidth").toInt(), mSettings.value("LeftSideButtonHeight").toInt());
    leftButtonContainer->setIconSize(QSize(mSettings.value("LeftButtonSize").toInt(), mSettings.value("LeftButtonSize").toInt()));

    //Create action
    const QIcon recentProjectIcon(mSettings.value("RecentFileIcon").toString());
    QAction* recentProjectBTN = new QAction(recentProjectIcon, "", nullptr);
    recentProjectBTN->setCheckable(true);
    recentProjectBTN->setChecked(true);
    mLeftActions.push_back(recentProjectBTN);
    connect(recentProjectBTN, &QAction::triggered, this, [this] { updateReadOnlyField(mExistant, true); leftViewSelected(mLeftActions[0]); });

    const QIcon GameTemplateIcon(mSettings.value("NewProjectFileIcon").toString());
    QAction* gameTemplateBTN = new QAction(GameTemplateIcon, "", nullptr);
    gameTemplateBTN->setCheckable(true);
    mLeftActions.push_back(gameTemplateBTN);
    connect(gameTemplateBTN, &QAction::triggered, this, [this] { updateReadOnlyField(mNewProject, false); leftViewSelected(mLeftActions[1]); });

    //Add widget to container
    mCenter->addLayout(leftSide, mSettings.value("LeftSideStretch").toInt());
    leftSide->addWidget(leftWindow);
    leftButtonContainer->addAction(recentProjectBTN);
    leftButtonContainer->addAction(gameTemplateBTN);
}

void WidgetProjectHub::initRightSide()
{
    QVBoxLayout* rightSide = new QVBoxLayout();

    mCenter->addLayout(rightSide, mSettings.value("RightSideStretch").toInt());

    // View one
    {
        RightView* viewOne = new RightView("Right", QMargins(mSettings.value("RightGridMarginLeft").toInt(),
                                                             mSettings.value("RightGridMarginTop").toInt(),
                                                             mSettings.value("RightGridMarginRight").toInt(),
                                                             mSettings.value("RightGridMarginBottom").toInt()));
        viewOne->setLayout(viewOne->mGrid);
        rightSide->addWidget(viewOne);

        for (unsigned int i = 0; i < mRecentProjects.size(); i++)
        {
            Widget::WidgetSelectable* proj = new WidgetSelectable(mSettings, mRecentProjects[i].mName, mSettings.value("ProjDefaultIcon").toString());

            installEventFilter(proj);
            installEventFilter(&proj->mText);

            if (i == 0)
            {
                proj->setClicked(true);
                mExistant = mRecentProjects[0];
            }


            int row = i / mSettings.value("Max_recent_project_saved").toInt();
            int column = i % mSettings.value("Max_recent_project_saved").toInt();

            viewOne->mGrid->addWidget(proj, row, column, Qt::AlignLeft | Qt::AlignTop);
            viewOne->mProjectSelectionWidget.push_back(proj);
        }

        viewOne->mGrid->setRowStretch(viewOne->mGrid->rowCount(), mSettings.value("RightGridStretch").toInt());
        viewOne->mGrid->setColumnStretch(viewOne->mGrid->columnCount(), mSettings.value("RightGridStretch").toInt());
        viewOne->setVisible(true);

        mRightWidgets.push_back(viewOne);
    }

    // View two
    {
        RightView* viewTwo = new RightView("Right", QMargins(mSettings.value("RightGridMarginLeft").toInt(),
                                                             mSettings.value("RightGridMarginTop").toInt(),
                                                             mSettings.value("RightGridMarginRight").toInt(),
                                                             mSettings.value("RightGridMarginBottom").toInt()));
        rightSide->addWidget(viewTwo);


        QHBoxLayout* vLayout = new QHBoxLayout();
        vLayout->setContentsMargins(0, 0, 0, 0);

        viewTwo->setLayout(vLayout);
        vLayout->addLayout(viewTwo->mGrid);


        {
            WidgetSelectableDescription* proj = new WidgetSelectableDescription(mSettings,
                                                                                mSettings.value("BlankProjectName").toString(),
                                                                                mSettings.value("BlankProjectIcon").toString(),
                                                                                "BlankProj",
                                                                                *vLayout);

            proj->setClicked(true);
            installEventFilter(proj);
            installEventFilter(&proj->mText);


            viewTwo->mGrid->addWidget(proj, 0, 0, Qt::AlignLeft | Qt::AlignTop);
            viewTwo->mProjectSelectionWidget.push_back(proj);

            mSelectedProject = proj;
        }

        {
            WidgetSelectableDescription* proj = new WidgetSelectableDescription(mSettings,
                                                                                mSettings.value("FPSProjectName").toString(),
                                                                                mSettings.value("FPSProjectIcon").toString(),
                                                                                "FPSProj",
                                                                                *vLayout);
            proj->setClicked(false);
            installEventFilter(proj);
            installEventFilter(&proj->mText);

            viewTwo->mGrid->addWidget(proj, 0, 1, Qt::AlignLeft | Qt::AlignTop);
            viewTwo->mProjectSelectionWidget.push_back(proj);
        }

        viewTwo->mGrid->setRowStretch(viewTwo->mGrid->rowCount(), mSettings.value("RightGridStretch").toInt());
        viewTwo->mGrid->setColumnStretch(viewTwo->mGrid->columnCount(), mSettings.value("RightGridStretch").toInt());
        viewTwo->setVisible(false);

        mRightWidgets.push_back(viewTwo);
    }

    mCurrentRightWidget = mRightWidgets[0];
}

void WidgetProjectHub::initBottomForm()
{
    //Create bottom container
    QWidget* bottomWindow = new QWidget();
    bottomWindow->setObjectName("Bottom");
    bottomWindow->setFixedHeight(mSettings.value("BottomWindowHeight").toInt());
    

    QVBoxLayout* bottomVerticaltalLayout = new QVBoxLayout(bottomWindow);
    QHBoxLayout* bottomHorizontalLayout = new QHBoxLayout();
    

    QFormLayout* leftForm = new QFormLayout();
    leftForm->setContentsMargins(QMargins(mSettings.value("BottomLeftFormMarginLeft").toInt(),
                                            mSettings.value("BottomLeftFormMarginTop").toInt(),
                                            mSettings.value("BottomLeftFormMarginRight").toInt(),
                                            mSettings.value("BottomLeftFormMarginBottom").toInt()));
    
    //Create button
    QLabel* projectPathLabel = new QLabel(mSettings.value("ProjectLocationText").toString());
    projectPathLabel->setObjectName("ProjectPath");

    mProjectPath = new QLineEdit(mExistant.mFolder);
    mProjectPath->setReadOnly(true);
    mProjectPath->setObjectName("ProjectPath");
    connect(mProjectPath, &QLineEdit::textChanged, this, &WidgetProjectHub::pathChanged);
    

    const QIcon folderIcon(mSettings.value("FolderIcon").toString());
    mBrowseAction = new QAction(folderIcon, mSettings.value("FolderTip").toString());
    connect(mBrowseAction, &QAction::triggered, this, &WidgetProjectHub::browseFolder);
    
    

    QFormLayout* rightForm = new QFormLayout();
    rightForm->setContentsMargins(QMargins(mSettings.value("BottomRightFormMarginLeft").toInt(),
                                            mSettings.value("BottomRightFormMarginTop").toInt(),
                                            mSettings.value("BottomRightFormMarginRight").toInt(),
                                            mSettings.value("BottomRightFormMarginBottom").toInt()));
    

    QLabel* projectNameLabel = new QLabel(mSettings.value("ProjectNameText").toString());
    projectNameLabel->setObjectName("ProjectName");

    mProjectName = new QLineEdit(mExistant.mName);
    mProjectName->setObjectName("ProjectName");
    mProjectName->setReadOnly(true);
    connect(mProjectName, &QLineEdit::textChanged, this, &WidgetProjectHub::projectChanged);
    

    QToolBar* bottomToolBar = new QToolBar("ProjectButton");
    bottomToolBar->setLayoutDirection(Qt::RightToLeft);
    bottomToolBar->setOrientation(Qt::Horizontal);
    bottomToolBar->setObjectName("ProjectButtons");
    bottomToolBar->setContentsMargins(QMargins( mSettings.value("BottomToolBarMarginLeft").toInt(),
                                                mSettings.value("BottomToolBarMarginTop").toInt(),
                                                mSettings.value("BottomToolBarMarginRight").toInt(),
                                                mSettings.value("BottomToolBarMarginBottom").toInt()));

    QAction* cancel = new QAction(mSettings.value("BottomToolBarCancel").toString());
    QAction* cancel2 = new QAction(mSettings.value("BottomToolBarCancel").toString());
    cancel2->setVisible(false);
    mProjectToolBarActionView1.push_back(cancel);
    mProjectToolBarActionView2.push_back(cancel2);
    connect(cancel, &QAction::triggered, this, &WidgetProjectHub::close);
    connect(cancel2, &QAction::triggered, this, &WidgetProjectHub::close);


    QAction* open = new QAction(mSettings.value("BottomToolBarOpen").toString());
    mProjectToolBarActionView1.push_back(open);
    connect(open, &QAction::triggered, this, &WidgetProjectHub::openProject);

    QAction* browse = new QAction(mSettings.value("BottomToolBarBrowse").toString());
    mProjectToolBarActionView1.push_back(browse);
    connect(browse, &QAction::triggered, this, &WidgetProjectHub::browseProject);

    QAction* create = new QAction(mSettings.value("BottomToolBarCreate").toString());
    create->setVisible(false);
    mProjectToolBarActionView2.push_back(create);
    connect(create, &QAction::triggered, this, &WidgetProjectHub::createProject);
    

    //Add widget to container
    installEventFilter(bottomWindow);
    mWindow->addWidget(bottomWindow);
    bottomVerticaltalLayout->addLayout(bottomHorizontalLayout);
    bottomHorizontalLayout->addLayout(leftForm, mSettings.value("BottomLeftStretch").toInt());
    leftForm->addRow(projectPathLabel, mProjectPath);
    mProjectPath->addAction(mBrowseAction, QLineEdit::TrailingPosition);
    mBrowseAction->setVisible(false);
    bottomHorizontalLayout->addLayout(rightForm, mSettings.value("BottomRightStretch").toInt());
    rightForm->addRow(projectNameLabel, mProjectName);

    bottomVerticaltalLayout->addWidget(bottomToolBar);
    bottomToolBar->addAction(cancel);
    bottomToolBar->addAction(cancel2);
    bottomToolBar->addAction(open);
    bottomToolBar->widgetForAction(open)->setObjectName("Blue");
    bottomToolBar->addAction(browse);
    bottomToolBar->addAction(create);
    bottomToolBar->widgetForAction(create)->setObjectName("Blue");
}

bool WidgetProjectHub::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (!(obj == mProjectPath && mProjectPath->hasFocus()))
            mProjectPath->clearFocus();

        if (!(obj == mProjectName && mProjectName->hasFocus()))
            mProjectName->clearFocus();

        if (obj->objectName() == "ProjectSelectionWidget" || obj->objectName() == "ProjectWidgetLabel")
        {
            for (unsigned int i = 0; i < mCurrentRightWidget->mProjectSelectionWidget.size(); i++)
            {
                if (obj == mCurrentRightWidget->mProjectSelectionWidget[i] || obj == &mCurrentRightWidget->mProjectSelectionWidget[i]->mText)
                {
                    mCurrentRightWidget->mProjectSelectionWidget[i]->setClicked(true);

                    if (mCurrentRightWidget == mRightWidgets[0])
                    {
                        (*mCurrentProject).mFolder = mRecentProjects[i].mFolder;
                        (*mCurrentProject).mName = mRecentProjects[i].mName;

                        mProjectPath->setText(mCurrentProject->mFolder);
                        mProjectName->setText(mCurrentProject->mName);
                    }
                    else
                        mSelectedProject = mCurrentRightWidget->mProjectSelectionWidget[i];
                }
                else
                    mCurrentRightWidget->mProjectSelectionWidget[i]->setClicked(false);
            }
        }
    }
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = (QKeyEvent*)event;
        if (keyEvent->key() == Qt::Key_Escape)
            close();
    }

    return QObject::eventFilter(obj, event);
}

void WidgetProjectHub::leftViewSelected(QAction* pAction)
{
    for (unsigned int i = 0; i < mLeftActions.size(); i++)
    {
        if (mLeftActions[i] == pAction)
        {
            mLeftActions[i]->setChecked(true);
            mCurrentRightWidget = mRightWidgets[i];
            mCurrentRightWidget->setVisible(true);
        }
        else
        {
            mLeftActions[i]->setChecked(false);
            mRightWidgets[i]->setVisible(false);
        }
    }

    mProjectPath->setText(mCurrentProject->mFolder);
    mProjectName->setText(mCurrentProject->mName);

    if (pAction == mLeftActions[0])
    {
        for (unsigned int i = 0; i < mProjectToolBarActionView2.size(); i++)
            mProjectToolBarActionView2[i]->setVisible(false);

        for (unsigned int i = 0; i < mProjectToolBarActionView1.size(); i++)
            mProjectToolBarActionView1[i]->setVisible(true);
    }
    else
    {
        for (unsigned int i = 0; i < mProjectToolBarActionView1.size(); i++)
            mProjectToolBarActionView1[i]->setVisible(false);

        for (unsigned int i = 0; i < mProjectToolBarActionView2.size(); i++)
            mProjectToolBarActionView2[i]->setVisible(true);
    }
}

void WidgetProjectHub::updateReadOnlyField(Data::ProjectLocation& pProject, bool pReadOnly)
{
    if (pReadOnly)
    {
        mBrowseAction->setVisible(false);
        mProjectPath->setReadOnly(true);
        mProjectPath->setStyleSheet(" QLineEdit[readOnly=true]:focus { border: 0.04em solid #2f2f2f; } ");

        mProjectName->setReadOnly(true);
        mProjectName->setStyleSheet(" QLineEdit[readOnly=true]:focus { border: 0.04em solid #2f2f2f; } ");
    }
    else
    {
        mBrowseAction->setVisible(true);
        mProjectPath->setReadOnly(false);
        mProjectPath->setStyleSheet(" QLineEdit[readOnly=false]:focus { border: 0.04em solid #0070e0; } ");

        mProjectName->setReadOnly(false);
        mProjectName->setStyleSheet(" QLineEdit[readOnly=false]:focus { border: 0.04em solid #0070e0; } ");
    }

    mCurrentProject = &pProject;
}

void WidgetProjectHub::pathChanged()
{
    mCurrentProject->mFolder = mProjectPath->text();
}

void WidgetProjectHub::browseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        mProjectPath->setText(dir);
    setFocus();
}

void WidgetProjectHub::projectChanged()
{
    mCurrentProject->mName = mProjectName->text();
}

void WidgetProjectHub::openProject()
{
    QString str = mCurrentProject->mFolder + "/" + mCurrentProject->mName + "." + Define::PROJECT_EXTENSION;
    if (!QFile::exists(str))
        return;

    writeSaveData();
    close();
    QApplication::exit(Define::EEditorState::OPEN_EDITOR);
}

void WidgetProjectHub::browseProject()
{
    std::string filter = "Project Files (*." + std::string(Define::PROJECT_EXTENSION) + ")";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr(filter.c_str()));
    if (!fileName.isEmpty())
    {
        QFileInfo file(fileName);
        mExistant.mFolder = file.absolutePath();
        mExistant.mName = file.completeBaseName();
        openProject();
    }
}

void WidgetProjectHub::createProject()
{
    QString location = mCurrentProject->mFolder + "/" + mCurrentProject->mName;

    QString tmp = mCurrentProject->mFolder;
    mCurrentProject->mFolder = location;

    QDir path(location);
    if (!path.exists())
        path.mkdir(".");

    QString str = location + "/" + mCurrentProject->mName + "." + Define::PROJECT_EXTENSION;
    if (QFile::exists(str))
    {
        QMessageBox msgBox(QMessageBox::Question,
                            "",
                            "The project already exists. \nDo you want to overwrite it?\n",
                            QMessageBox::Yes | QMessageBox::No, this,
                            Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

        if (msgBox.exec() == QMessageBox::Yes)
            QFile::remove(str);
        else
        {
            mCurrentProject->mFolder = tmp;
            return;
        }
    }

    QFile file(str);
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox msgBox(QMessageBox::Warning,
            "",
            "The directory path doesn\'t exist.\n",
            QMessageBox::Ok, this,
            Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        msgBox.exec();

        mCurrentProject->mFolder = tmp;
        return;
    }
    file.remove();

    Editor::Utils::copyAndReplaceFolderContents(QDir::currentPath() + "/" + mSettings.value("TemplateDirectory").toString() + mSelectedProject->mText.text(), mCurrentProject->mFolder);
    QFile::rename(mCurrentProject->mFolder + "/" + mSettings.value("DefaultProjectNameWithExtension").toString(), str);

    writeSaveData();
    close();
    QApplication::exit(Define::EEditorState::OPEN_EDITOR);
}