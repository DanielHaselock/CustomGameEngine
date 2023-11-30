#include <QtCore/qdir.h>
#include "Editor/Widget/WidgetDropUI.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtoolbar.h>
#include "Editor/Widget/WidgetStyleUIButton.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include "Tools/Utils/Define.h"
#include "Editor/Utils/Utils.h"
#include <Editor/Widget/WidgetCollapse.h>
#include <QtWidgets/qcheckbox.h>
#include <Editor/Widget/WidgetCollapseUI.h>
#include "Editor/Widget/WidgetDropInput.h"
#include <QtWidgets/qcolordialog.h>
#include "Editor/Widget/WidgetStyleUIImageButton.h"
#include "Editor/Widget/WidgetStyleUITextButton.h"

using namespace Editor::Widget;

WidgetStyleUIButton::WidgetStyleUIButton(Editor::UI::Button& pResource, QWidget* pParent) :
    QDialog(pParent),
    mResource(pResource),
    mSettings("Resources/Editor/Config/StyleUIEN.ini", QSettings::IniFormat)
{
    setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));

    setWindowDecoration();
    initWindowCloseButton();
    initCentralWidget();
    initBottom();
}

void WidgetStyleUIButton::setWindowDecoration()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(mSettings.value("MainWindowWidth").toInt(), mSettings.value("MainWindowHeight").toInt());
}

void WidgetStyleUIButton::initWindowCloseButton()
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
    connect(quitAtion, &QAction::triggered, this, &WidgetStyleUIButton::close);
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

void WidgetStyleUIButton::initCentralWidget()
{
    ads::CDockManager* mDockManager;
    
    QFlags<ads::CDockManager::eConfigFlag> flags = ads::CDockManager::eConfigFlag::DragPreviewIsDynamic | ads::CDockManager::eConfigFlag::DockAreaHasUndockButton;
    ads::CDockManager::setConfigFlags(flags);
    mDockManager = new ads::CDockManager(this);
    mDockManager->setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("DockingStyleSheet").toString()));

    mWindow->addWidget(mDockManager);
    mWindow->setObjectName("central");


    ads::CDockWidget* left = new ads::CDockWidget("Button");
    left->setWidget(new Editor::Widget::WidgetStyleUIImageButton(mResource));

    ads::CDockWidget* right = new ads::CDockWidget("Text");
    right->setWidget(new Editor::Widget::WidgetStyleUITextButton(mResource));

    ads::CDockAreaWidget* areaCentral = mDockManager->addDockWidget(ads::CenterDockWidgetArea, left);
    mDockManager->addDockWidgetTabToArea(right, areaCentral);

    left->toggleView(true);
}

void WidgetStyleUIButton::initBottom()
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
    connect(cancel, &QAction::triggered, this, &WidgetStyleUIButton::quit);

    QAction* accept = new QAction(mSettings.value("BottomToolBarAccept").toString());
    mBottomToolBar->addAction(accept);
    mBottomToolBar->widgetForAction(accept)->setObjectName("Blue");
    connect(accept, &QAction::triggered, this, &WidgetStyleUIButton::quit);
}

bool WidgetStyleUIButton::eventFilter(QObject* obj, QEvent* event)
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

void WidgetStyleUIButton::quit()
{
    close();
    setResult(0);
}

WidgetDoubleSpinBox* WidgetStyleUIButton::setSpinBox(std::string pText, int& pAxisVal, QGridLayout* pLayout, int row, int column)
{
    pLayout->addWidget(new QLabel(pText.c_str()), row, column, Qt::AlignLeft);
    pLayout->addWidget(new QLabel(" "), row, column + 1, Qt::AlignRight);


    WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pAxisVal, -100, 100, 1);
    pLayout->addWidget(spinBox, row, column + 2, 1, 4, Qt::AlignLeft);

    return spinBox;
}

void WidgetStyleUIButton::setTexture(QBoxLayout& widLayout, Editor::UI::UIStyle& style)
{
    // Model name
    QLabel* mModelName = new QLabel(style.pTextPath);
    widLayout.addWidget(mModelName);

    // Model path
    WidgetDropUI* mModelPath = new WidgetDropUI(*mModelName);
    mModelPath->setText(style.pTextPath);
    mModelPath->setReadOnly(true);
    widLayout.addWidget(mModelPath);
    connect(mModelPath, &WidgetDropUI::textChanged, this, [&style, mModelPath]
    {
        style.pTextPath = mModelPath->text();
    });

    const QIcon folderIcon(mSettings.value("FolderIcon").toString());
    QAction* browseAction = new QAction(folderIcon, mSettings.value("FolderTip").toString());
    mModelPath->addAction(browseAction, QLineEdit::TrailingPosition);
    connect(browseAction, &QAction::triggered, this, [this, mModelName, mModelPath, &style]
        {
            QString Defaultpath = service(Data::ProjectLocation).mFolder;

            std::string filter = "Texture Files (*.png *.jpg *.jpeg)";
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
            {
                QFileInfo info(fileName);

                mModelName->setText(info.baseName());
                mModelPath->setText(fileName);

                style.pTextPath = fileName;
            }
        });
}

void WidgetStyleUIButton::setColor(QBoxLayout& widLayout, Editor::UI::UIStyle& style)
{
    QColorDialog* dialog = new QColorDialog(style.color);
    dialog->setOption(QColorDialog::NoButtons, true);
    dialog->setOption(QColorDialog::ShowAlphaChannel, true);
    dialog->setWindowFlags(Qt::Widget);
    connect(dialog, &QColorDialog::currentColorChanged, this, [this, dialog, &style]
    {
        style.color = dialog->currentColor();
    });

    //Hide color panel child
    for (int i = 0; i < dialog->children().count(); i++)
    {
        std::string list = dialog->children().at(i)->metaObject()->className();

        if (list != "QColorPicker" && list != "QColorLuminancePicker" && list != "QColorShower")
        {

            QWidget* wid = dynamic_cast<QWidget*>(dialog->children().at(i));
            if (wid != nullptr)
                wid->hide();
        }

        if (list == "QColorShower")
        {
            QWidget* obj = ((QWidget*)dialog->children().at(i));
            for (int j = 0; j < obj->children().count(); j++)
            {
                std::string childList = obj->children().at(j)->metaObject()->className();

                if (childList != "QGridLayout" && childList != "QColorShowLabel")
                {
                    if (j > 7 && j < 16)
                        continue;

                    QWidget* wid = dynamic_cast<QWidget*>(obj->children().at(j));
                    if (wid != nullptr)
                        wid->hide();
                }
            }
            obj->adjustSize();
        }
    }
    dialog->adjustSize();

    widLayout.addWidget(dialog);
}