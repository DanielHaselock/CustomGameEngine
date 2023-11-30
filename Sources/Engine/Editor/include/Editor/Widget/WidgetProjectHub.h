#pragma once
#include <QtWidgets/qwidget.h>
#include <QtCore/qsettings.h>
#include <QtWidgets/qboxlayout.h>

#include "Editor/Data/ProjectLocation.h"
#include "Editor/Widget/WidgetSelectable.h"
#include "Editor/Widget/WidgetSelectableDescription.h"


namespace Editor::Widget
{
	struct RightView : public QWidget
	{
		QVector<Widget::WidgetSelectable*> mProjectSelectionWidget;
		QGridLayout* mGrid = new QGridLayout();

		RightView(const QString& pName, const QMargins& margin);
	};

	class WidgetProjectHub : public QWidget
	{
	public:
		QSettings mSettings;

		Data::ProjectLocation mNewProject;
		Data::ProjectLocation mExistant;
		Data::ProjectLocation* mCurrentProject = &mExistant;
		QVector<Data::ProjectLocation> mRecentProjects;

		QBoxLayout* mWindow = nullptr;
		QBoxLayout* mCenter = nullptr;

		QVector<QAction*> mLeftActions;
		QVector<RightView*> mRightWidgets;

		WidgetSelectable* mSelectedProject = nullptr;
		RightView* mCurrentRightWidget = nullptr;

		QVector<QAction*> mProjectToolBarActionView1;
		QVector<QAction*> mProjectToolBarActionView2;

		QLineEdit* mProjectPath = nullptr;
		QLineEdit* mProjectName = nullptr;
		QAction* mBrowseAction = nullptr;

		WidgetProjectHub();

		void setTheme();
		void setStandardProjectLocation();
		void setWindowDecoration();
		void registerMetaType();
		void readSavedData();
		void writeSaveData();

		void initWindowControlButtons();
		void initCenter();
		void initLeftCenter();
		void initRightSide();
		void initBottomForm();

		bool eventFilter(QObject* obj, QEvent* event);

		//Qevent
		void leftViewSelected(QAction* pAction);
		void updateReadOnlyField(Data::ProjectLocation& pProject, bool pReadOnly);
		void pathChanged();
		void browseFolder();
		void projectChanged();
		void openProject();
		void browseProject();
		void createProject();
	};
}