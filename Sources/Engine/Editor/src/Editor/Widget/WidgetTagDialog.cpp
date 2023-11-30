#include "Editor/Widget/WidgetTagDialog.h"
#include "Editor/Resources/Loader/StyleSheetLoader.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qlineedit.h>
#include "Editor/Utils/Utils.h"
#include "QtWidgets/qpushbutton.h"

using namespace Editor::Widget;

WidgetTagDialog::WidgetTagDialog(std::list<std::string>& pTags, QWidget* pParent) :
    QDialog(pParent),
    mTags(pTags),
    mSettings("Resources/Editor/Config/StyleUIEN.ini", QSettings::IniFormat)
{
    setStyleSheet(Resources::Loaders::StyleSheetLoader::loadStyleSheet(mSettings.value("StyleSheet").toString()));

    setWindowDecoration();
    initWindowCloseButton();
    initCentralWidget();
    initBottom();
}

void WidgetTagDialog::setWindowDecoration()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(mSettings.value("MainWindowWidth").toInt(), mSettings.value("MainWindowHeight").toInt());
}

void WidgetTagDialog::initWindowCloseButton()
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
    connect(quitAtion, &QAction::triggered, this, &WidgetTagDialog::close);
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

void WidgetTagDialog::initCentralWidget()
{
    central = new QWidget();
    mWindow->addWidget(central);
    mWindow->setObjectName("central");
    
    QVBoxLayout* vBox = new QVBoxLayout();
    central->setLayout(vBox);

    QPushButton* button = new QPushButton("Add Tags", nullptr);
    button->setObjectName("AddComponent");
    button->setStyleSheet("padding-left: 60px; padding-right: 60px;");
    button->setMinimumHeight(25);
    button->setMaximumHeight(25);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    vBox->addWidget(button, 0, Qt::AlignTop | Qt::AlignHCenter);
    connect(button, &QPushButton::pressed, this, [this, vBox]
    {
        addTag(*vBox);
    });

    for (auto& tag : mTags)
        loadTag(*vBox, tag);
}

void WidgetTagDialog::loadTag(QVBoxLayout& pLayout, std::string& pTag)
{
    QLineEdit* tag = new QLineEdit();
    tag->setText(pTag.c_str());
    tag->setMinimumHeight(25);
    tag->setMaximumHeight(25);
    tag->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tag->setObjectName("GameObject");
    
    connect(tag, &QLineEdit::textChanged, this, [this, tag, &pTag]
    {
        pTag = Utils::qStringToStdString(tag->text());
    });

    const QIcon folderIcon(mSettings.value("DeleteIcon").toString());
    QAction* browseAction = new QAction(folderIcon, mSettings.value("DeleteTip").toString());
    tag->addAction(browseAction, QLineEdit::TrailingPosition);
    connect(browseAction, &QAction::triggered, this, [this, tag, &pTag, &pLayout]
    {
            pLayout.removeWidget(tag);
            mTags.remove(pTag);
            counter--;

            delete tag;
    });

    pLayout.insertWidget(counter, tag, 0, Qt::AlignTop);
    pLayout.addStretch();

    counter++;
}

void WidgetTagDialog::addTag(QVBoxLayout& pLayout)
{
    mTags.push_back("");
    std::string& pTag = mTags.back();

    QLineEdit* tag = new QLineEdit();
    tag->setMinimumHeight(25);
    tag->setMaximumHeight(25);
	tag->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	tag->setObjectName("GameObject");

    connect(tag, &QLineEdit::textChanged, this, [this, &pTag, tag]
    {
        pTag = Utils::qStringToStdString(tag->text());
    });

    const QIcon folderIcon(mSettings.value("DeleteIcon").toString());
    QAction* browseAction = new QAction(folderIcon, mSettings.value("DeleteTip").toString());
    tag->addAction(browseAction, QLineEdit::TrailingPosition);
    connect(browseAction, &QAction::triggered, this, [this, tag, &pTag, &pLayout]
    {
        pLayout.removeWidget(tag);
        mTags.remove(pTag);
        counter--;

        delete tag;
    });

    pLayout.insertWidget(counter, tag, 0, Qt::AlignTop);
    pLayout.addStretch();

    counter++;
}

void WidgetTagDialog::initBottom()
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
    connect(accept, &QAction::triggered, this, &WidgetTagDialog::quit);
}

bool WidgetTagDialog::eventFilter(QObject* obj, QEvent* event)
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

void WidgetTagDialog::quit()
{
    close();
    setResult(0);
}