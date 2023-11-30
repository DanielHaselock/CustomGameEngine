#pragma once
#include <QtWidgets/qwidget.h>
#include "Game/Data/Actor.h"
#include <QtCore/qsettings.h>
#include <QtGui/qstandarditemmodel.h>
#include <QtWidgets/qboxlayout.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qpushbutton.h>
#include "Game/Component/AComponent.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlineedit.h>
#include "Game/Component/CPModel.h"
#include <QtWidgets/qfiledialog.h>
#include "Game/Component/CPBoxCollider.h"
#include "Game/Component/CPParticle.h"
#include "Game/Component/CPCapsuleCollider.h"
#include "Editor/Widget/WidgetDoubleSpinBox.h"
#include "Game/Component/CPCamera.h"
#include "Game/Component/CPScript.h"
#include "Editor/Data/FlowLayout.h"
#include <Game/Component/CPSound.h>
#include <Game/Component/CPSoundListener.h>
#include <Game/Component/CPUI.h>
#include <Game/Component/CPAnimator.h>
#include <Game/Component/CPDirLight.h>
#include <Game/Component/CPPointLight.h>
#include <Game/Component/CPSpotLight.h>
#include <Game/Component/CPDecal.h>
#include <Game/Component/CPComplexCollider.h>

namespace Editor::Widget
{
	class WidgetDropInput;

	class WidgetInspectorData : public QWidget
	{
	public:
		QStandardItem* mItem = nullptr;
		QVBoxLayout* mLayout = new QVBoxLayout();
		QSettings& mSetting;
		QPushButton* mCreateComponent;
		QLineEdit* mGameObjectName = nullptr;

		QList<QLineEdit*> mTags;
		QVBoxLayout* grid = nullptr;

		QLabel* mModelName;
		WidgetDropInput* mModelPath;

		QLabel* mMatName;
		WidgetDropInput* mMatPath;

		QLabel* mParticleName;
		WidgetDropInput* mParticlePath;

		QLabel* mDecalName;
		WidgetDropInput* mDecalPath;

		QLabel* mAnimatorName;
		WidgetDropInput* mAnimatorPath;

		QLabel* mScriptName;
		WidgetDropInput* mScriptPath;

		QLabel* mSoundName;
		WidgetDropInput* mSoundPath;

		QLabel* mUIName;
		WidgetDropInput* mUIPath;

		WidgetInspectorData(QSettings& pSetting, QStandardItem* pItem);
		QAction* createAction(const QIcon& pIcon, const QString& pTitle, const QString& pStatusTip, const QKeySequence::StandardKey& pShortcut, QWidget& pWidget);
			
		void setObjectNameEdit();
		void setObjectTagName();
		void setCreateButton();

		bool eventFilter(QObject* obj, QEvent* event) override;
	
		void initTransforms();
		void setIndividualTransformAxisScrollbar(std::string Text, float& pAxisVal, QGridLayout* layout, Game::Component::CPTransform& pTransform, int row, int column);
		void updateChildModel(QStandardItem* pItem);
	
		void setIndividualTransformAxisScrollbarWithRotation(QGridLayout* layout, Game::Component::CPTransform& pTransform, int row, int column);

		void createCamera();
		void addCamera(Game::Component::CPCamera* pCamera);

		void createModel();
		void addModel(Game::Component::CPModel* pModel);
		void setModel(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);
		void setModel(QFileInfo& info);
		void setMat(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);

		void createAnimator();
		void addAnimator(Game::Component::CPAnimator* pAnimator);
		void setAnimator(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);

		void createParticle();
		void addParticle(Game::Component::CPParticle* pParticle);
		void setParticle(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);
		void setParticle(QFileInfo& info);

		void createDecal();
		void addDecal(Game::Component::CPDecal* pDecal);
		void setDecal(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);

		void createBoxCollider();
		void addBoxCollider(Game::Component::CPBoxCollider* pBody);

		void createCapsuleCollider();
		void addCapsuleCollider(Game::Component::CPCapsuleCollider* pBody);

		void createComplexCollider();
		void addComplexCollider(Game::Component::CPComplexCollider* pBody);

		void createScript();
		void addScript(Game::Component::CPScript* pScript);
		void setScript(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);
		void setScript(QFileInfo& info);

		void createSound();
		void addSound(Game::Component::CPSound* pSound);
		void setSound(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);
		void setSound(QFileInfo& info);

		void createSoundListener();
		void addSoundListener(Game::Component::CPSoundListener* pSound);

		void createUI();
		void addUI(Game::Component::CPUI* pUI);
		void setUI(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath);
		void setUI(QFileInfo& info);

		void createDirLight();
		void addDirLight(Game::Component::CPDirLight* pDirLight);

		void createPointLight();
		void addPointLight(Game::Component::CPPointLight* pPointLight);

		void createSpotLight();
		void addSpotLight(Game::Component::CPSpotLight* pSpotLight);

		WidgetDoubleSpinBox* setSpinBox(std::string pText, float& pAxisVal, QGridLayout* pLayout, int row, int column);
		void WidgetInspectorData::setIndividualBoxAxisScrollbar(std::string pText, float& pAxisVal, QGridLayout* pLayout, Game::Component::ACollider& pCol, int row, int column);
		void WidgetInspectorData::setIndividualBoxAxisScrollbar(std::string pText, float& pAxisVal, QGridLayout* pLayout, Game::Component::CPDecal& pCol, int row, int column);
		QColor setColorWidget(const QColor& pColor);

	};
}