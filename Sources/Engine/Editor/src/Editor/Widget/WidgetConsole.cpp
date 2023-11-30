#include "Editor/Widget/WidgetConsole.h"
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qcheckbox.h>
#include "Tools/Time/Date.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>


using namespace Editor::Widget;

WidgetConsole* WidgetConsole::gInstance = nullptr;

WidgetConsole::WidgetConsole(QWidget* pParent) : QPlainTextEdit(pParent)
{
	if (gInstance != nullptr)
		return;

	gInstance = this;

	setObjectName("console");
	setReadOnly(true);

	//connect(this, &QPlainTextEdit::textChanged, this, &WidgetConsole::unlockPrint);
}

WidgetConsole::~WidgetConsole()
{
	gInstance = nullptr;
}

void WidgetConsole::setColor(const Qt::GlobalColor& pColor)
{
	if (pLastColor == pColor)
		return;

	QTextCharFormat tf = currentCharFormat();
	tf.setForeground(QBrush(QColor(pColor)));
	setCurrentCharFormat(tf);

	pLastColor = pColor;
}

void WidgetConsole::setColor(int pR, int pG, int pB)
{
	QColor aColor(pR, pG, pB);
	if (pLastColor == aColor)
		return;

	QTextCharFormat tf = currentCharFormat();
	tf.setForeground(QBrush(aColor));
	setCurrentCharFormat(tf);

	pLastColor = aColor;
}

void WidgetConsole::invokeMsg(const std::string& pMessage, int pR, int pG, int pB)
{
	std::string str = "  [" + Tools::Time::Date::getTimeAsString() + "]  " + pMessage;
	QMetaObject::invokeMethod(&service(Editor::Widget::WidgetEditor), [str, pR, pG, pB]
		{
			std::unique_lock lk(gInstance->mtx);

			QColor aColor(pR, pG, pB);
			if (gInstance->pLastColor != aColor)
			{
				QTextCharFormat tf = gInstance->currentCharFormat();
				tf.setForeground(QBrush(aColor));
				gInstance->setCurrentCharFormat(tf);

				gInstance->pLastColor = aColor;
			}

			gInstance->appendPlainText(str.c_str());
		});
}

void WidgetConsole::play()
{
	if (mClearPlay)
		clear();
}

WidgetConsoleApp::WidgetConsoleApp(QSettings& pSettings, QWidget* pParent) : ads::CDockWidget("Console")
{
	QWidget* content = new QWidget(pParent);
	setWidget(content);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	layout->setContentsMargins(5, 0, 5, 5);
	layout->setSpacing(0);
	content->setLayout(layout);
	
	QToolBar* toolbar = new QToolBar();
	toolbar->setObjectName("console");
	layout->addWidget(toolbar);
	
	QAction* action = toolbar->addAction(QIcon(pSettings.value("IconClear").toString()), "");
	connect(action, &QAction::triggered, this, [this] { mConsole.clear(); });
	
	QCheckBox* checkBox = new QCheckBox(pSettings.value("ConsoleClear").toString());
	checkBox->setChecked(true);
	connect(checkBox, &QCheckBox::clicked, this, [this] { mConsole.mClearPlay = !mConsole.mClearPlay; });
	toolbar->addWidget(checkBox);

	QCheckBox* debugCheck = new QCheckBox(pSettings.value("ConsoleDebug").toString());
	debugCheck->setChecked(true);
	connect(debugCheck, &QCheckBox::clicked, this, [this] { mConsole.mShowDebug = !mConsole.mShowDebug; });
	toolbar->addWidget(debugCheck);

	QCheckBox* infoCheck = new QCheckBox(pSettings.value("ConsoleInfo").toString());
	infoCheck->setChecked(true);
	connect(infoCheck, &QCheckBox::clicked, this, [this] { mConsole.mShowInfo = !mConsole.mShowInfo; });
	toolbar->addWidget(infoCheck);

	QCheckBox* warningCheck = new QCheckBox(pSettings.value("ConsoleWarning").toString());
	warningCheck->setChecked(true);
	connect(warningCheck, &QCheckBox::clicked, this, [this] { mConsole.mShowWarning = !mConsole.mShowWarning; });
	toolbar->addWidget(warningCheck);

	QCheckBox* errorCheck = new QCheckBox(pSettings.value("ConsoleError").toString());
	errorCheck->setChecked(true);
	connect(errorCheck, &QCheckBox::clicked, this, [this] { mConsole.mShowError = !mConsole.mShowError; });
	toolbar->addWidget(errorCheck);
	
	layout->addWidget(&mConsole);
}

void WidgetConsoleApp::play()
{
	mConsole.play();
}