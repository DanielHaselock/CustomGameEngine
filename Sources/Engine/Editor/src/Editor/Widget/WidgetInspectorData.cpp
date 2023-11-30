#include "Editor/Widget/WidgetInspectorData.h"
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qmenu.h>
#include "Editor/Widget/WidgetGameObjectTreeItem.h"
#include "Editor/Widget/WidgetCollapse.h"
#include "Editor/Widget/WidgetDropInput.h"
#include "Editor/Widget/WidgetMenuSperarator.h"
#include <QtWidgets/qfiledialog.h>
#include "Editor/Utils/Utils.h"
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qformlayout.h>
#include "Game/Utils/ComponentType.h"
#include <QtWidgets/qcombobox.h>
#include "Physics/Data/TypeRigidBody.h"
#include "EngineCore/EventSystem/Event.h"
#include "Maths/Utils.h"
#include <QtWidgets/qslider.h>
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Scripting/ScriptInterpreter.h"
#include "Editor/Data/ProjectLocation.h"
#include "Editor/Widget/WidgetCollapseTag.h"
#include <QtWidgets/qsizepolicy.h>
#include "Editor/Widget/WidgetStyleSkyBox.h"
#include "Editor/Widget/WidgetColor.h"
#include "Editor/Widget/WidgetClickedLabel.h"
#include <Editor/Widget/WidgetTagDialog.h>
#include <QtWidgets/qmessagebox.h>
#include <Editor/Widget/WidgetSocketDialog.h>
#include <Editor/Widget/WidgetMeshMatDialog.h>

#undef far
#undef near

using namespace Editor::Widget;

WidgetInspectorData::WidgetInspectorData(QSettings& pSetting, QStandardItem* pItem)
	: mItem(pItem), mSetting(pSetting)
{
	setObjectName("Inspector");

	mLayout->setSpacing(5);
	mLayout->setContentsMargins(0, 0, 0, 5);
	setLayout(mLayout);

	setObjectNameEdit();
	setObjectTagName();
	setCreateButton();

	initTransforms();
}

void WidgetInspectorData::setObjectNameEdit()
{
	mGameObjectName = new QLineEdit();
	mGameObjectName->setObjectName("GameObject");
	mGameObjectName->setText(mItem->text());
	connect(mGameObjectName, &QLineEdit::textChanged, this, [=]
		{
			mItem->setText(mGameObjectName->text());
		});

	mLayout->addWidget(mGameObjectName, 0, Qt::AlignTop);
}

void WidgetInspectorData::setObjectTagName()
{
	WidgetGameObjectTreeItem* item = (WidgetGameObjectTreeItem*)mItem;

	WidgetCollapseTag* wiz = new WidgetCollapseTag(mSetting, *item->mActor, QIcon(mSetting.value("InspectorTagIcon").toString()), mSetting.value("InspectorTagTitle").toString());
	wiz->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	wiz->setObjectName("Component");
	mLayout->addWidget(wiz);

	grid = new QVBoxLayout();
	grid->setContentsMargins(15, 5, 5, 5);
	grid->setSizeConstraint(QLayout::SetNoConstraint);

	QPushButton* button = new QPushButton("Set Tags", nullptr);
	button->setObjectName("AddComponent");
	button->setMinimumHeight(0);
	button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	grid->addWidget(button, 0, Qt::AlignTop);
	connect(button, &QPushButton::pressed, this, [this]
	{
		WidgetGameObjectTreeItem* item = (WidgetGameObjectTreeItem*)mItem;
		Editor::Widget::WidgetTagDialog dialog(item->mActor->mTags);
		installEventFilter(&dialog);
		dialog.exec();

	});

	wiz->setContentLayout(*grid);
}

void WidgetInspectorData::setCreateButton()
{
	// Add Component
	mCreateComponent = new QPushButton("Add Component", nullptr);
	mCreateComponent->setObjectName("AddComponent");
	mLayout->addWidget(mCreateComponent, 0, Qt::AlignTop);

	QMenu* menu = new QMenu("Hello world", mCreateComponent);
	menu->setStyleSheet("Qmenu:item { background-color: #383838; } QMenu { height: 480px; } ");
	menu->installEventFilter(this);
	mCreateComponent->setMenu(menu);

	//Camera
	QAction* newAction = nullptr;
	{
		WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSetting, mSetting.value("CategoryInspectorCamera").toString(), mSetting.value("SeparatorIcon").toString(), menu);
		menu->addAction(sepa->mAction);
	}

	newAction = createAction(QIcon(mSetting.value("MenuInspectorCameraIcon").toString()), mSetting.value("MenuInspectorCamera").toString(), mSetting.value("MenuInspectorCameraTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createCamera);

	//Renderer
	newAction = nullptr;
	{
		WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSetting, mSetting.value("CategoryInspectorModel").toString(), mSetting.value("SeparatorIcon").toString(), menu);
		menu->addAction(sepa->mAction);
	}

	newAction = createAction(QIcon(mSetting.value("MenuInspectorModelIcon").toString()), mSetting.value("MenuInspectorModel").toString(), mSetting.value("MenuInspectorModelTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createModel);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorAnimatorIcon").toString()), mSetting.value("MenuInspectorAnimator").toString(), mSetting.value("MenuInspectorAnimatorTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createAnimator);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorParticleIcon").toString()), mSetting.value("MenuInspectorParticle").toString(), mSetting.value("MenuInspectorParticleTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createParticle);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorDecalIcon").toString()), mSetting.value("MenuInspectorDecal").toString(), mSetting.value("MenuInspectorDecalTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createDecal);

	//Physic
	{
		WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSetting, mSetting.value("CategoryInspectorPhysic").toString(), mSetting.value("SeparatorIcon").toString(), menu);
		menu->addAction(sepa->mAction);
	}

	newAction = createAction(QIcon(mSetting.value("MenuInspectorPhysicBoxColliderIcon").toString()), mSetting.value("MenuInspectorPhysicBoxCollider").toString(), mSetting.value("MenuInspectorPhysicBoxColliderTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createBoxCollider);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorPhysicCapsuleColliderIcon").toString()), mSetting.value("MenuInspectorPhysicCapsuleCollider").toString(), mSetting.value("MenuInspectorPhysicCapsuleColliderTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createCapsuleCollider);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorPhysicComplexColliderIcon").toString()), mSetting.value("MenuInspectorPhysicComplexCollider").toString(), mSetting.value("MenuInspectorPhysicComplexColliderTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createComplexCollider);

	//Script
	{
		WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSetting, mSetting.value("CategoryInspectorScripting").toString(), mSetting.value("SeparatorIcon").toString(), menu);
		menu->addAction(sepa->mAction);
	}

	newAction = createAction(QIcon(mSetting.value("MenuInspectorScriptingIcon").toString()), mSetting.value("MenuInspectorScripting").toString(), mSetting.value("MenuInspectorSriptingTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createScript);

	//Sound
	{
		WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSetting, mSetting.value("CategoryInspectorSound").toString(), mSetting.value("SeparatorIcon").toString(), menu);
		menu->addAction(sepa->mAction);
	}

	newAction = createAction(QIcon(mSetting.value("MenuInspectorSoundIcon").toString()), mSetting.value("MenuInspectorSound").toString(), mSetting.value("MenuInspectorSoundTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createSound);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorSoundListenerIcon").toString()), mSetting.value("MenuInspectorSoundListener").toString(), mSetting.value("MenuInspectorSoundListenerTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createSoundListener);

	//UI
	{
		WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSetting, mSetting.value("CategoryInspectorUI").toString(), mSetting.value("SeparatorIcon").toString(), menu);
		menu->addAction(sepa->mAction);
	}

	newAction = createAction(QIcon(mSetting.value("MenuInspectorUIIcon").toString()), mSetting.value("MenuInspectorUI").toString(), mSetting.value("MenuInspectorUITips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createUI);

	//Light
	{
		WidgetMenuSeperator* sepa = new WidgetMenuSeperator(mSetting, mSetting.value("CategoryInspectorLight").toString(), mSetting.value("SeparatorIcon").toString(), menu);
		menu->addAction(sepa->mAction);
	}

	newAction = createAction(QIcon(mSetting.value("MenuInspectorDirLightIcon").toString()), mSetting.value("MenuInspectorDirLight").toString(), mSetting.value("MenuInspectorDirLightTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createDirLight);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorPointLightIcon").toString()), mSetting.value("MenuInspectorPointLight").toString(), mSetting.value("MenuInspectorPointLightTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createPointLight);

	newAction = createAction(QIcon(mSetting.value("MenuInspectorSpotLightIcon").toString()), mSetting.value("MenuInspectorSpotLight").toString(), mSetting.value("MenuInspectorSpotLightTips").toString(), QKeySequence::UnknownKey, *menu);
	connect(newAction, &QAction::triggered, this, &WidgetInspectorData::createSpotLight);
}

QAction* WidgetInspectorData::createAction(const QIcon& pIcon, const QString& pTitle, const QString& pStatusTip, const QKeySequence::StandardKey& pShortcut, QWidget& pWidget)
{
	QAction* newAction = new QAction(pIcon, pTitle, this);
	newAction->setShortcuts(pShortcut);
	newAction->setStatusTip(pStatusTip);
	pWidget.addAction(newAction);
	return newAction;
}

bool WidgetInspectorData::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::Show && obj == mCreateComponent->menu())
	{
		QPoint pos = mapToGlobal(mCreateComponent->parentWidget()->rect().bottomLeft());
		pos.setX(pos.x() + ((mCreateComponent->width() / 2) - (mCreateComponent->menu()->width() / 2)));

		mCreateComponent->menu()->move(pos);
		return true;
	}
	return false;
}

void WidgetInspectorData::initTransforms()
{
	WidgetGameObjectTreeItem* item = (WidgetGameObjectTreeItem*)mItem;

	std::list<Game::Component::AComponent*>& transforms = item->mActor->mComponents[Game::Utils::ComponentType::Transform];
	for (Game::Component::AComponent* Component : transforms)
	{
		Game::Component::CPTransform* transform = (Game::Component::CPTransform*)Component;

		WidgetCollapse* wiz = new WidgetCollapse(mSetting, *item->mActor, *transform, Game::Utils::ComponentType::Transform, QIcon(mSetting.value("InspectorTransformIcon").toString()), mSetting.value("InspectorTransformTitle").toString());
		mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
		wiz->setObjectName("Component");

		QGridLayout* grid = new QGridLayout();
		grid->setContentsMargins(15, 5, 5, 5);
		grid->setVerticalSpacing(1);

		//Position
		{
			QLabel* position = new QLabel("Position");
			grid->addWidget(position, 0, 0, Qt::AlignLeft);

			setIndividualTransformAxisScrollbar("X", transform->mLocalPosition.x, grid, *transform, 0, 1);
			setIndividualTransformAxisScrollbar("Y", transform->mLocalPosition.y, grid, *transform, 0, 3);
			setIndividualTransformAxisScrollbar("Z", transform->mLocalPosition.z, grid, *transform, 0, 5);
		}

		//Rotation
		{
			QLabel* position = new QLabel("Rotation");
			grid->addWidget(position, 1, 0, Qt::AlignLeft);

			setIndividualTransformAxisScrollbarWithRotation(grid, *transform, 1, 1);
		}

		//Scale
		{
			QLabel* position = new QLabel("Scale");
			grid->addWidget(position, 2, 0, Qt::AlignLeft);

			setIndividualTransformAxisScrollbar("X", transform->mLocalScale.x, grid, *transform, 2, 1);
			setIndividualTransformAxisScrollbar("Y", transform->mLocalScale.y, grid, *transform, 2, 3);
			setIndividualTransformAxisScrollbar("Z", transform->mLocalScale.z, grid, *transform, 2, 5);
		}

		wiz->setContentLayout(*grid);
	}
}

void WidgetInspectorData::setIndividualTransformAxisScrollbar(std::string pText, float& pAxisVal, QGridLayout* pLayout, Game::Component::CPTransform& pTransform, int row, int column)
{
	QLabel* axis = new QLabel(pText.c_str());
	pLayout->addWidget(axis, row, column, Qt::AlignRight);

	WidgetDoubleSpinBox* SpinBox = new WidgetDoubleSpinBox(pAxisVal, INT64_MIN, INT64_MAX);
	pLayout->addWidget(SpinBox, row, column + 1, Qt::AlignLeft);
	connect(SpinBox, &WidgetDoubleSpinBox::valueChanged, this, [SpinBox, &pAxisVal, &pTransform, this]
	{
		if (SpinBox->manuelEvent)
		{
			SpinBox->manuelEvent = false;
			return;
		}

			pAxisVal = SpinBox->value();
			((WidgetGameObjectTreeItem*)mItem)->mActor->updateWorldTransformEditor(&pTransform);
			updateChildModel(mItem);

		});

	((WidgetGameObjectTreeItem*)mItem)->mActor->mValueChanged.subscribe([SpinBox, &pAxisVal]
	{
		QMetaObject::invokeMethod(SpinBox, [SpinBox, &pAxisVal]
			{
				SpinBox->manuelEvent = true;
				SpinBox->setValue(pAxisVal);
			});
	});
}

void WidgetInspectorData::updateChildModel(QStandardItem* pItem)
{
	int numChildren = pItem->rowCount();
	for (int i = 0; i < numChildren; ++i)
	{
		WidgetGameObjectTreeItem* currItem = (WidgetGameObjectTreeItem*)pItem->child(i, 0);
		currItem->mActor->updateWorldTransformEditor(currItem->mActor->getTransform());
		updateChildModel(currItem);
	}
}

void WidgetInspectorData::setIndividualTransformAxisScrollbarWithRotation(QGridLayout* pLayout, Game::Component::CPTransform& pTransform, int row, int column)
{
	QLabel* axisX = new QLabel("X");
	pLayout->addWidget(axisX, 1, 1, Qt::AlignRight);

	WidgetDoubleSpinBox* SpinBoxX = new WidgetDoubleSpinBox(0, INT64_MIN, INT64_MAX);
	pLayout->addWidget(SpinBoxX, 1, 2, Qt::AlignLeft);

	QLabel* axisY = new QLabel("Y");
	pLayout->addWidget(axisY, 1, 3, Qt::AlignRight);

	WidgetDoubleSpinBox* SpinBoxY = new WidgetDoubleSpinBox(0, INT64_MIN, INT64_MAX);
	pLayout->addWidget(SpinBoxY, 1, 4, Qt::AlignLeft);

	QLabel* axisZ = new QLabel("Z");
	pLayout->addWidget(axisZ, 1, 5, Qt::AlignRight);

	WidgetDoubleSpinBox* SpinBoxZ = new WidgetDoubleSpinBox(0, INT64_MIN, INT64_MAX);
	pLayout->addWidget(SpinBoxZ, 1, 6, Qt::AlignLeft);

	connect(SpinBoxX, &WidgetDoubleSpinBox::valueChanged, this, [=, &pTransform]
	{
		if (SpinBoxX->manuelEvent)
		{
			SpinBoxX->manuelEvent = false;
			return;
		}

			Maths::FVector3 vec = { (float)SpinBoxX->value(), (float)SpinBoxY->value(), (float)SpinBoxZ->value() };
			pTransform.mLocalRotation = Maths::FQuaternion(vec);
			((WidgetGameObjectTreeItem*)mItem)->mActor->updateWorldTransformEditor(&pTransform);
			updateChildModel(mItem);
		});

	connect(SpinBoxY, &WidgetDoubleSpinBox::valueChanged, this, [=, &pTransform]
	{
		if (SpinBoxY->manuelEvent)
		{
			SpinBoxY->manuelEvent = false;
			return;
		}

			Maths::FVector3 vec = { (float)SpinBoxX->value(), (float)SpinBoxY->value(),(float)SpinBoxZ->value() };
			pTransform.mLocalRotation = Maths::FQuaternion(vec);
			((WidgetGameObjectTreeItem*)mItem)->mActor->updateWorldTransformEditor(&pTransform);
			updateChildModel(mItem);
		});

	connect(SpinBoxZ, &WidgetDoubleSpinBox::valueChanged, this, [=, &pTransform]
	{
		if (SpinBoxZ->manuelEvent)
		{
			SpinBoxZ->manuelEvent = false;
			return;
		}

		Maths::FVector3 vec = { (float)SpinBoxX->value(), (float)SpinBoxY->value(), (float)SpinBoxZ->value() };
		pTransform.mLocalRotation = Maths::FQuaternion(vec);
		((WidgetGameObjectTreeItem*)mItem)->mActor->updateWorldTransformEditor(&pTransform);
	});

	((WidgetGameObjectTreeItem*)mItem)->mActor->mValueChanged.subscribe([SpinBoxX, SpinBoxY, SpinBoxZ, &pTransform]
		{
			Maths::FVector3 vec = pTransform.mLocalRotation.eulerAngles();

			QMetaObject::invokeMethod(SpinBoxX, [SpinBoxX, SpinBoxY, SpinBoxZ, &pTransform, vec]
				{
					SpinBoxX->manuelEvent = true;
					SpinBoxX->setValue(vec.x);
				});

			QMetaObject::invokeMethod(SpinBoxY, [SpinBoxX, SpinBoxY, SpinBoxZ, &pTransform, vec]
				{
					SpinBoxY->manuelEvent = true;
					SpinBoxY->setValue(vec.y);
				});

			QMetaObject::invokeMethod(SpinBoxZ, [SpinBoxX, SpinBoxY, SpinBoxZ, &pTransform, vec]
				{
					SpinBoxZ->manuelEvent = true;
					SpinBoxZ->setValue(vec.z);
				});
		});
}

void WidgetInspectorData::createCamera()
{
	if (((WidgetGameObjectTreeItem*)mItem)->mActor->mComponents.find(Game::Utils::ComponentType::Camera) != ((WidgetGameObjectTreeItem*)mItem)->mActor->mComponents.end())
		if (((WidgetGameObjectTreeItem*)mItem)->mActor->mComponents[Game::Utils::ComponentType::Camera].size() != 0)
			return;
	

	Game::Component::CPCamera* newCamera = new Game::Component::CPCamera(service(Rendering::Renderer::VK::VKRenderer).mWindow->mWidth,
		service(Rendering::Renderer::VK::VKRenderer).mWindow->mHeight,
		((WidgetGameObjectTreeItem*)mItem)->mActor->getTransform()->mLocalPosition,
		((WidgetGameObjectTreeItem*)mItem)->mActor->getTransform()->mLocalRotation,
		((WidgetGameObjectTreeItem*)mItem)->mActor->mValueChangedFromEditor);
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Camera, newCamera);

	addCamera(newCamera);
}

void WidgetInspectorData::addCamera(Game::Component::CPCamera* pCamera)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pCamera, Game::Utils::ComponentType::Camera, QIcon(mSetting.value("MenuInspectorCameraIcon").toString()), mSetting.value("InspectorCameraTitle").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setContentsMargins(15, 5, 5, 5);
	widLayout->setVerticalSpacing(1);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pCamera->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pCamera->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// Combo box
	{
		QComboBox* typeCollision = new QComboBox();

		typeCollision->addItem("Perspective", QVariant((int)0));
		typeCollision->addItem("Orthogonal", QVariant((int)1));

		connect(typeCollision, &QComboBox::currentIndexChanged, this, [typeCollision, pCamera]
		{
			/*QVariant value = typeCollision->itemData(typeCollision->currentIndex());
			pCamera->setType((Physics::Data::TypeRigidBody)value.toInt());*/

			typeCollision->clearFocus();
		});

		widLayout->addWidget(new QLabel("Projection"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		widLayout->addWidget(typeCollision, 1, 2, 1, 4, Qt::AlignLeft);
	}

	// QSlider
	{
		QLabel* label = new QLabel(QString::number(pCamera->mFOV));

		QSlider* slider = new QSlider(Qt::Orientation::Horizontal);
		slider->setMinimum(0);
		slider->setMaximum(180);
		slider->setValue(pCamera->mFOV);

		connect(slider, &QSlider::valueChanged, this, [slider, label, pCamera]
		{
			label->setText(QString::number(slider->value()));

			pCamera->mFOV = slider->value();
			pCamera->updateProjection();
			slider->clearFocus();
		});

		widLayout->addWidget(new QLabel("Field of view"), 2, 0, Qt::AlignLeft);
		widLayout->addWidget(label, 2, 1, Qt::AlignRight);
		widLayout->addWidget(slider, 2, 2, 1, 4, Qt::AlignLeft);
	}

	//Clipping plane
	{	widLayout->addWidget(new QLabel("Clipping Planes"), 3, 0, Qt::AlignRight);
		
		{
			widLayout->addWidget(new QLabel("Near"), 3, 1, Qt::AlignRight);

			WidgetDoubleSpinBox* SpinBox = new WidgetDoubleSpinBox(pCamera->mNear, 0, INT64_MAX);
			widLayout->addWidget(SpinBox, 3, 2, 1, 4, Qt::AlignLeft);

			float& near = pCamera->mNear;
			connect(SpinBox, &WidgetDoubleSpinBox::valueChanged, this, [SpinBox, &near, pCamera]
			{
				near = SpinBox->value();
				pCamera->updateProjection();
			});
		}

		{
			widLayout->addWidget(new QLabel(" "), 4, 0, Qt::AlignRight);
			widLayout->addWidget(new QLabel("Far"), 4, 1, Qt::AlignRight);

			WidgetDoubleSpinBox* SpinBox = new WidgetDoubleSpinBox(pCamera->mFar, 0, INT64_MAX);
			widLayout->addWidget(SpinBox, 4, 2, 1, 4, Qt::AlignLeft);

			float& far = pCamera->mFar;
			connect(SpinBox, &WidgetDoubleSpinBox::valueChanged, this, [SpinBox, &far, pCamera]
			{
				far = SpinBox->value();
				pCamera->updateProjection();
			});
		}
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pCamera->useSkyBox ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Use Skybox"), 5, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 5, 1, Qt::AlignRight);
		widLayout->addWidget(box, 5, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
		{
			box->clearFocus();
			pCamera->useSkyBox = box->checkState() == Qt::CheckState::Checked;
		});
	}

	//Style button
	{
		QPushButton* button = new QPushButton("Set SkyBox", nullptr);
		button->setObjectName("AddComponent");
		widLayout->addWidget(button, 6, 0, 1, 4, Qt::AlignCenter);
		connect(button, &QPushButton::pressed, this, [this, button, wiz, pCamera]
		{
			Editor::Widget::WidgetStyleSkyBox dialog(*pCamera);
			installEventFilter(&dialog);
			if (dialog.exec() == 0)
			{
				std::string left = Utils::qStringToStdString(dialog.lineEdit[0]->text());
				std::string right = Utils::qStringToStdString(dialog.lineEdit[1]->text());
				std::string top = Utils::qStringToStdString(dialog.lineEdit[2]->text());
				std::string botton = Utils::qStringToStdString(dialog.lineEdit[3]->text());
				std::string front = Utils::qStringToStdString(dialog.lineEdit[4]->text());
				std::string back = Utils::qStringToStdString(dialog.lineEdit[5]->text());

				if (left.empty() || right.empty() || top.empty() || botton.empty() || front.empty() || back.empty())
					QMessageBox::warning(this, tr("Application"), tr("One or more field is empty"), QMessageBox::Ok);
				else
					pCamera->skybox.setTexture(left, right, top, botton, front, back);
			}
		});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::createModel()
{
	Game::Component::CPModel* newModel = new Game::Component::CPModel();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::MeshRenderer, newModel);

	addModel(newModel);
}

void WidgetInspectorData::addModel(Game::Component::CPModel* pModel)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pModel, Game::Utils::ComponentType::MeshRenderer, QIcon(mSetting.value("MenuInspectorModelIcon").toString()), mSetting.value("InspectorModelTitle").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QBoxLayout* widLayout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	widLayout->setSpacing(0);

	{
		//Model
		{
			// Model name
			mModelName = new QLabel(pModel->mName.c_str());
			widLayout->addWidget(mModelName);

			// Model path
			mModelPath = new WidgetDropInput(*mModelName, *pModel, Tools::Utils::PathParser::EFileType::MODEL);
			mModelPath->setText(pModel->mPath.c_str());
			mModelPath->setReadOnly(true);
			widLayout->addWidget(mModelPath);
			connect(mModelPath, &WidgetDropInput::textChanged, this, [=]
				{
					pModel->setModelWithPath(nullptr);
				});

			const QIcon folderIcon(mSetting.value("FolderIcon").toString());
			QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
			mModelPath->addAction(browseAction, QLineEdit::TrailingPosition);
			connect(browseAction, &QAction::triggered, this, [=]
				{
					setModel(pModel, mModelName, mModelPath);
				});

			//Socket name
			{
				QPushButton* button = new QPushButton("Show Socket Name", nullptr);
				button->setObjectName("AddComponent");
				button->setMinimumHeight(0);
				button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

				widLayout->addWidget(button);
				connect(button, &QPushButton::pressed, this, [this, pModel]
				{
					Editor::Widget::WidgetSocketDialog dialog(pModel);
					installEventFilter(&dialog);
					dialog.exec();
				});
			}
		}

		//Mat
		{
			// Mat name
			mMatName = new QLabel(pModel->mNameMat.c_str());
			widLayout->addWidget(mMatName);

			// Mat path
			mMatPath = new WidgetDropInput(*mMatName, *pModel, Tools::Utils::PathParser::EFileType::MATERIAL);
			mMatPath->setText(pModel->mPathMat.c_str());
			mMatPath->setReadOnly(true);
			widLayout->addWidget(mMatPath);
			connect(mMatPath, &WidgetDropInput::textChanged, this, [this, pModel]
				{
					if (!mMatPath->text().isEmpty())
						return;

					pModel->setMat(nullptr);

					mMatName->setText("WorldMaterial");
					mMatPath->setText("Resources/Engine/Material/WorldMaterial.mat");
				});

			const QIcon folderIcon(mSetting.value("FolderIcon").toString());
			QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
			mMatPath->addAction(browseAction, QLineEdit::TrailingPosition);
			connect(browseAction, &QAction::triggered, this, [=]
				{
					setMat(pModel, mMatName, mMatPath);
				});


			//Mesh Mat name
			{
				QPushButton* button = new QPushButton("Set Mesh Mat", nullptr);
				button->setObjectName("AddComponent");
				button->setMinimumHeight(0);
				button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

				widLayout->addWidget(button);
				connect(button, &QPushButton::pressed, this, [this, pModel]
				{
					Editor::Widget::WidgetMeshMatDialog dialog(pModel);
					installEventFilter(&dialog);
					dialog.exec();
				});
			}
		}

	}

	QWidget* container = new QWidget();
	container->setObjectName("Component");
	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(15, 5, 5, 5);
	grid->setVerticalSpacing(1);
	container->setLayout(grid);
	widLayout->addWidget(container);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pModel->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		grid->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pModel->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pModel->canReceiveDecal ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Receive Decal"), 1, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		grid->addWidget(box, 1, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pModel->canReceiveDecal = box->checkState() == Qt::CheckState::Checked;
			});
	}

	//Socket Connection
	{
		QLineEdit* socket = new QLineEdit();
		socket->setText(pModel->socketName.c_str());
		socket->setObjectName("GameObject");

		grid->addWidget(new QLabel("Socket"), 2, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 2, 1, Qt::AlignRight);
		grid->addWidget(socket, 2, 2, 1, 4, Qt::AlignLeft);
		
		connect(socket, &QLineEdit::textChanged, this, [this, pModel, socket]
		{
			pModel->setSocket(Utils::qStringToStdString(socket->text()));
		});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::setModel(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	std::string filter = "Model Files (*.fbx *.obj *.dae)";
	QFileDialog dialog(this, tr("Open Model"), Defaultpath, tr(filter.c_str()));
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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPModel*)pComponent)->setModel(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::setModel(QFileInfo& info)
{
	Game::Component::CPModel* newModel = new Game::Component::CPModel();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::MeshRenderer, newModel);

	addModel(newModel);

	mModelName->setText(info.baseName());
	mModelPath->setText(info.absoluteFilePath());
	newModel->setModel(Utils::qStringToStdString(info.baseName())
		, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
}

void WidgetInspectorData::setMat(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPModel*)pComponent)->setMat(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::createAnimator()
{
	Game::Component::CPAnimator* newAnimator = new Game::Component::CPAnimator();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Animator, newAnimator);

	addAnimator(newAnimator);
}

void WidgetInspectorData::addAnimator(Game::Component::CPAnimator* pAnimator)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pAnimator, Game::Utils::ComponentType::Animator, QIcon(mSetting.value("MenuInspectorAnimatorIcon").toString()), mSetting.value("InspectorAnimatorlTitle").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QBoxLayout* widLayout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	widLayout->setSpacing(0);

	//Animation
	{
		// Animation name
		mAnimatorName = new QLabel(pAnimator->mName.c_str());
		widLayout->addWidget(mAnimatorName);

		// Animation path
		mAnimatorPath = new WidgetDropInput(*mAnimatorName, *pAnimator, Tools::Utils::PathParser::EFileType::MODEL);
		mAnimatorPath->setText(pAnimator->mPath.c_str());
		mAnimatorPath->setReadOnly(true);
		widLayout->addWidget(mAnimatorPath);
		connect(mAnimatorPath, &WidgetDropInput::textChanged, this, [=]
		{
			pAnimator->setAnimation(nullptr);
		});

		const QIcon folderIcon(mSetting.value("FolderIcon").toString());
		QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
		mAnimatorPath->addAction(browseAction, QLineEdit::TrailingPosition);
		connect(browseAction, &QAction::triggered, this, [=]
		{
			setAnimator(pAnimator, mAnimatorName, mAnimatorPath);
		});
	}

	QWidget* container = new QWidget();
	container->setObjectName("Component");
	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(15, 5, 5, 5);
	grid->setVerticalSpacing(1);
	container->setLayout(grid);
	widLayout->addWidget(container);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pAnimator->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		grid->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pAnimator->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pAnimator->playOnAwake ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Play on awake"), 1, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		grid->addWidget(box, 1, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pAnimator->playOnAwake = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pAnimator->loop ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Loop"), 2, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 2, 1, Qt::AlignRight);
		grid->addWidget(box, 2, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pAnimator->loop = box->checkState() == Qt::CheckState::Checked;
				pAnimator->reset();
			});
	}

	{
		QLabel* axis = new QLabel("Model Component idx");
		grid->addWidget(axis, 3, 0, Qt::AlignRight);

		WidgetDoubleSpinBox* SpinBox = new WidgetDoubleSpinBox(pAnimator->cpModelIdx, -1, INT64_MAX, 1);
		grid->addWidget(SpinBox, 3, 1, Qt::AlignLeft);
		connect(SpinBox, &WidgetDoubleSpinBox::valueChanged, this, [SpinBox, pAnimator, this]
		{
			pAnimator->cpModelIdx = SpinBox->value();
			pAnimator->updateModel();
			pAnimator->updateAnimationFromModel();
		});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::setAnimator(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	std::string filter = "Animation Files (*.fbx *.dae)";
	QFileDialog dialog(this, tr("Open Animation"), Defaultpath, tr(filter.c_str()));
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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPAnimator*)pComponent)->setAnimation(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::createParticle()
{
	Game::Component::CPParticle* newParticle = new Game::Component::CPParticle();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Particle, newParticle);

	addParticle(newParticle);
}

void WidgetInspectorData::addParticle(Game::Component::CPParticle* pParticle)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pParticle, Game::Utils::ComponentType::Particle, QIcon(mSetting.value("MenuInspectorParticleIcon").toString()), mSetting.value("InspectorParticleTitle").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QBoxLayout* widLayout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	widLayout->setSpacing(0);

	//Animation
	{
		// Animation name
		mParticleName = new QLabel(pParticle->mName.c_str());
		widLayout->addWidget(mParticleName);

		// Animation path
		mParticlePath = new WidgetDropInput(*mParticleName, *pParticle, Tools::Utils::PathParser::EFileType::PARTICLE);
		mParticlePath->setText(pParticle->mPath.c_str());
		mParticlePath->setReadOnly(true);
		widLayout->addWidget(mParticlePath);
		connect(mParticlePath, &WidgetDropInput::textChanged, this, [=]
		{
			pParticle->setParticle(nullptr);
		});

		const QIcon folderIcon(mSetting.value("FolderIcon").toString());
		QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
		mParticlePath->addAction(browseAction, QLineEdit::TrailingPosition);
		connect(browseAction, &QAction::triggered, this, [=]
		{
			setParticle(pParticle, mParticleName, mParticlePath);
		});
	}

	QWidget* container = new QWidget();
	container->setObjectName("Component");
	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(15, 5, 5, 5);
	grid->setVerticalSpacing(1);
	container->setLayout(grid);
	widLayout->addWidget(container);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pParticle->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		grid->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pParticle->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pParticle->playOnAwake ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Play on awake"), 1, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		grid->addWidget(box, 1, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
		{
			box->clearFocus();
			pParticle->playOnAwake = box->checkState() == Qt::CheckState::Checked;
		});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pParticle->playInEditor ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Play in Editor"), 2, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 2, 1, Qt::AlignRight);
		grid->addWidget(box, 2, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [pParticle, box]
			{
				box->clearFocus();
				pParticle->playInEditor = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pParticle->loop ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Loop"), 3, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 3, 1, Qt::AlignRight);
		grid->addWidget(box, 3, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
		{
			box->clearFocus();
			pParticle->loop = box->checkState() == Qt::CheckState::Checked;
		});
	}

	// spin box
	{
		WidgetDoubleSpinBox* spinMass = setSpinBox("PlayBack Speed", pParticle->mPlaybackSpeed, grid, 4, 0);
		connect(spinMass, &WidgetDoubleSpinBox::valueChanged, this, [spinMass, pParticle]
		{
			pParticle->mPlaybackSpeed = spinMass->value();
		});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::setParticle(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	std::string filter = "Particle Files (*.pat)";
	QFileDialog dialog(this, tr("Open Particle"), Defaultpath, tr(filter.c_str()));
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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPParticle*)pComponent)->setParticle(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::setParticle(QFileInfo& info)
{
	Game::Component::CPParticle* newParticle = new Game::Component::CPParticle();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Particle, newParticle);

	addParticle(newParticle);

	mParticleName->setText(info.baseName());
	mParticlePath->setText(info.absoluteFilePath());
	newParticle->setParticle(Utils::qStringToStdString(info.baseName())
		, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
}

void WidgetInspectorData::createDecal()
{
	Game::Component::CPDecal* newDecal = new Game::Component::CPDecal(((WidgetGameObjectTreeItem*)mItem)->mActor->mValueChangedFromEditor);
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Decal, newDecal);

	addDecal(newDecal);
}

void WidgetInspectorData::addDecal(Game::Component::CPDecal* pDecal)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pDecal, Game::Utils::ComponentType::Decal, QIcon(mSetting.value("MenuInspectorDecalIcon").toString()), mSetting.value("MenuInspectorDecal").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QBoxLayout* widLayout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	widLayout->setSpacing(0);

	//Decal
	{
		// Decal name
		mDecalName = new QLabel(pDecal->mName.c_str());
		widLayout->addWidget(mDecalName);

		// Decal path
		mDecalPath = new WidgetDropInput(*mDecalName, *pDecal, Tools::Utils::PathParser::EFileType::TEXTURE);
		mDecalPath->setText(pDecal->mPath.c_str());
		mDecalPath->setReadOnly(true);
		widLayout->addWidget(mDecalPath);
		connect(mDecalPath, &WidgetDropInput::textChanged, this, [=]
			{
				pDecal->setDecalWithPath(nullptr);
			});

		const QIcon folderIcon(mSetting.value("FolderIcon").toString());
		QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
		mDecalPath->addAction(browseAction, QLineEdit::TrailingPosition);
		connect(browseAction, &QAction::triggered, this, [=]
			{
				setDecal(pDecal, mDecalName, mDecalPath);
			});
	}

	QWidget* container = new QWidget();
	container->setObjectName("Component");
	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(15, 5, 5, 5);
	grid->setVerticalSpacing(1);
	container->setLayout(grid);
	widLayout->addWidget(container);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pDecal->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		grid->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pDecal->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	//Size
	{
		QLabel* position = new QLabel("Size");
		grid->addWidget(position, 1, 0, Qt::AlignLeft);

		setIndividualBoxAxisScrollbar("X", pDecal->mSize.x, grid, *pDecal, 1, 1);
		setIndividualBoxAxisScrollbar("Y", pDecal->mSize.y, grid, *pDecal, 1, 3);
		setIndividualBoxAxisScrollbar("Z", pDecal->mSize.z, grid, *pDecal, 1, 5);
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::setDecal(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	std::string filter = "Image Files (*.png *.jpg *.bmp *.jpeg)";
	QFileDialog dialog(this, tr("Open Image"), Defaultpath, tr(filter.c_str()));
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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPDecal*)pComponent)->setDecal(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::createBoxCollider()
{
	WidgetGameObjectTreeItem* item = ((WidgetGameObjectTreeItem*)mItem);

	Maths::FVector3 position = item->mActor->getTransform()->mWorldPosition;
	Maths::FVector3 size = item->mActor->getTransform()->mWorldScale;

	Game::Component::CPBoxCollider* newBody = new Game::Component::CPBoxCollider(position, size, (void*)item->mActor);
	item->mActor->addComponent(Game::Utils::ComponentType::Collider, newBody);

	addBoxCollider(newBody);
}

void WidgetInspectorData::addBoxCollider(Game::Component::CPBoxCollider* pBody)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pBody, Game::Utils::ComponentType::Collider, QIcon(mSetting.value("MenuInspectorPhysicBoxColliderIcon").toString()), mSetting.value("InspectorPhysicBoxColliderTitle").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setContentsMargins(15, 5, 5, 5);
	widLayout->setVerticalSpacing(1);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pBody->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pBody->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// Combo box
	{
		QComboBox* typeCollision = new QComboBox();

		typeCollision->addItem("Dynamic", QVariant((int)Physics::Data::TypeRigidBody::E_DYNAMIC));
		typeCollision->addItem("Static", QVariant((int)Physics::Data::TypeRigidBody::E_STATIC));
		typeCollision->addItem("Kinematic", QVariant((int)Physics::Data::TypeRigidBody::E_KINEMATIC));
		typeCollision->addItem("Trigger", QVariant((int)Physics::Data::TypeRigidBody::E_TRIGGER));

		if (pBody->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
			typeCollision->setCurrentIndex((int)pBody->mType - 1);
		else
			typeCollision->setCurrentIndex((int)pBody->mType);
		connect(typeCollision, &QComboBox::currentIndexChanged, this, [typeCollision, pBody]
			{
				QVariant value = typeCollision->itemData(typeCollision->currentIndex());
				pBody->mType = ((Physics::Data::TypeRigidBody)value.toInt());
				pBody->setType((Physics::Data::TypeRigidBody)value.toInt());
				typeCollision->clearFocus();
			});

		widLayout->addWidget(new QLabel("Type"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		widLayout->addWidget(typeCollision, 1, 2, 1, 4, Qt::AlignLeft);
	}

	// Mass
	{
		WidgetDoubleSpinBox* spinMass = setSpinBox("Mass", pBody->mMass, widLayout, 2, 0);
		connect(spinMass, &WidgetDoubleSpinBox::valueChanged, this, [spinMass, pBody]
			{
				pBody->setMass(spinMass->value());
			});
	}

	//Center
	{
		QLabel* position = new QLabel("Center");
		widLayout->addWidget(position, 3, 0, Qt::AlignLeft);

		setIndividualBoxAxisScrollbar("X", pBody->mCenter.x, widLayout, *pBody, 3, 1);
		setIndividualBoxAxisScrollbar("Y", pBody->mCenter.y, widLayout, *pBody, 3, 3);
		setIndividualBoxAxisScrollbar("Z", pBody->mCenter.z, widLayout, *pBody, 3, 5);
	}

	//Size
	{
		QLabel* position = new QLabel("Size");
		widLayout->addWidget(position, 4, 0, Qt::AlignLeft);

		setIndividualBoxAxisScrollbar("X", pBody->mBaseSize.x, widLayout, *pBody, 4, 1);
		setIndividualBoxAxisScrollbar("Y", pBody->mBaseSize.y, widLayout, *pBody, 4, 3);
		setIndividualBoxAxisScrollbar("Z", pBody->mBaseSize.z, widLayout, *pBody, 4, 5);
	}
	//BlockRotations
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pBody->getLockX() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("BlockRotation"), 5, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel("X"), 5, 1, Qt::AlignRight);
		widLayout->addWidget(box, 5, 2, 1, 1, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pBody->getLockX() = box->checkState() == Qt::CheckState::Checked;
			});

		QCheckBox* box2 = new QCheckBox();
		box2->setCheckState(pBody->getLockY() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Y"), 5, 3, Qt::AlignRight);
		widLayout->addWidget(box2, 5, 4, 1, 1, Qt::AlignLeft);
		connect(box2, &QCheckBox::stateChanged, this, [=]
			{
				box2->clearFocus();
				pBody->getLockY() = box2->checkState() == Qt::CheckState::Checked;
			});


		QCheckBox* box3 = new QCheckBox();
		box3->setCheckState(pBody->getLockZ() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Z"), 5, 5, Qt::AlignRight);
		widLayout->addWidget(box3, 5, 6, 1, 1, Qt::AlignLeft);
		connect(box3, &QCheckBox::stateChanged, this, [=]
			{
				box3->clearFocus();
				pBody->getLockZ() = box3->checkState() == Qt::CheckState::Checked;
			});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::createCapsuleCollider()
{
	WidgetGameObjectTreeItem* item = ((WidgetGameObjectTreeItem*)mItem);

	Maths::FVector3 position = item->mActor->getTransform()->mWorldPosition;
	float Radius = Maths::fmax(item->mActor->getTransform()->mWorldScale.x, item->mActor->getTransform()->mWorldScale.z);
	float Height = item->mActor->getTransform()->mWorldScale.y;
	Game::Component::CPCapsuleCollider* newBody = new Game::Component::CPCapsuleCollider(position, Radius, Height, (void*)item->mActor);
	item->mActor->addComponent(Game::Utils::ComponentType::Collider, newBody);

	addCapsuleCollider(newBody);
}

void WidgetInspectorData::addCapsuleCollider(Game::Component::CPCapsuleCollider* pBody)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pBody, Game::Utils::ComponentType::Collider, QIcon(mSetting.value("MenuInspectorPhysicCapsuleColliderIcon").toString()), mSetting.value("InspectorPhysicCapsuleColliderTitle").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setContentsMargins(15, 5, 5, 5);
	widLayout->setVerticalSpacing(1);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pBody->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pBody->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// Combo box
	{
		QComboBox* typeCollision = new QComboBox();

		typeCollision->addItem("Dynamic", QVariant((int)Physics::Data::TypeRigidBody::E_DYNAMIC));
		typeCollision->addItem("Static", QVariant((int)Physics::Data::TypeRigidBody::E_STATIC));
		typeCollision->addItem("Kinematic", QVariant((int)Physics::Data::TypeRigidBody::E_KINEMATIC));
		typeCollision->addItem("Trigger", QVariant((int)Physics::Data::TypeRigidBody::E_TRIGGER));

		if (pBody->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
			typeCollision->setCurrentIndex((int)pBody->mType - 1);
		else
			typeCollision->setCurrentIndex((int)pBody->mType);
		connect(typeCollision, &QComboBox::currentIndexChanged, this, [typeCollision, pBody]
			{
				QVariant value = typeCollision->itemData(typeCollision->currentIndex());
				pBody->mType = ((Physics::Data::TypeRigidBody)value.toInt());
				pBody->setType((Physics::Data::TypeRigidBody)value.toInt());
				typeCollision->clearFocus();
			});

		widLayout->addWidget(new QLabel("Type"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		widLayout->addWidget(typeCollision, 1, 2, 1, 4, Qt::AlignLeft);
	}

	// Mass
	{
		WidgetDoubleSpinBox* spinMass = setSpinBox("Mass", pBody->mMass, widLayout, 2, 0);
		connect(spinMass, &WidgetDoubleSpinBox::valueChanged, this, [spinMass, pBody]
			{
				pBody->setMass(spinMass->value());
			});
	}

	//Center
	{
		QLabel* position = new QLabel("Center");
		widLayout->addWidget(position, 3, 0, Qt::AlignLeft);

		setIndividualBoxAxisScrollbar("X", pBody->mCenter.x, widLayout, *pBody, 3, 1);
		setIndividualBoxAxisScrollbar("Y", pBody->mCenter.y, widLayout, *pBody, 3, 3);
		setIndividualBoxAxisScrollbar("Z", pBody->mCenter.z, widLayout, *pBody, 3, 5);
	}

	//Radius
	{
		QLabel* position = new QLabel("Radius");
		widLayout->addWidget(position, 4, 0, Qt::AlignLeft);

		setIndividualBoxAxisScrollbar(" ", pBody->mRadius, widLayout, *pBody, 4, 1);
	}

	//Height
	{
		QLabel* position = new QLabel("Height");
		widLayout->addWidget(position, 5, 0, Qt::AlignLeft);

		setIndividualBoxAxisScrollbar(" ", pBody->mHeight, widLayout, *pBody, 5, 1);
	}

	//BlockRotations
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pBody->mLockRotX ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("BlockRotation"), 6, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel("X"), 6, 1, Qt::AlignRight);
		widLayout->addWidget(box, 6, 2, 1, 1, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pBody->mLockRotX = box->checkState() == Qt::CheckState::Checked;
			});

		QCheckBox* box2 = new QCheckBox();
		box2->setCheckState(pBody->getLockY() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Y"), 6, 3, Qt::AlignRight);
		widLayout->addWidget(box2, 6, 4, 1, 1, Qt::AlignLeft);
		connect(box2, &QCheckBox::stateChanged, this, [=]
			{
				box2->clearFocus();
				pBody->getLockY() = box2->checkState() == Qt::CheckState::Checked;
			});


		QCheckBox* box3 = new QCheckBox();
		box3->setCheckState(pBody->getLockZ() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Z"), 6, 5, Qt::AlignRight);
		widLayout->addWidget(box3, 6, 6, 1, 1, Qt::AlignLeft);
		connect(box3, &QCheckBox::stateChanged, this, [=]
			{
				box3->clearFocus();
				pBody->getLockZ() = box3->checkState() == Qt::CheckState::Checked;
			});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::createComplexCollider()
{
	WidgetGameObjectTreeItem* item = ((WidgetGameObjectTreeItem*)mItem);

	Maths::FVector3 position = item->mActor->getTransform()->mLocalPosition;
	Maths::FQuaternion rotation = item->mActor->getTransform()->mLocalRotation;
	Maths::FVector3 size = item->mActor->getTransform()->mLocalScale;

	Game::Component::CPComplexCollider* newBody = new Game::Component::CPComplexCollider(position, rotation, size, item->mActor->mComponents[Game::Utils::ComponentType::MeshRenderer], (void*)item->mActor);
	item->mActor->addComponent(Game::Utils::ComponentType::Collider, newBody);

	addComplexCollider(newBody);
}

void WidgetInspectorData::addComplexCollider(Game::Component::CPComplexCollider* pBody)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pBody, Game::Utils::ComponentType::Collider, QIcon(mSetting.value("MenuInspectorPhysicComplexColliderIcon").toString()), mSetting.value("MenuInspectorPhysicComplexCollider").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setContentsMargins(15, 5, 5, 5);
	widLayout->setVerticalSpacing(1);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pBody->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pBody->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// Combo box
	{
		QComboBox* typeCollision = new QComboBox();

		typeCollision->addItem("Static", QVariant((int)Physics::Data::TypeRigidBody::E_STATIC));
		typeCollision->setCurrentIndex(0);
		
		widLayout->addWidget(new QLabel("Type"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		widLayout->addWidget(typeCollision, 1, 2, 1, 4, Qt::AlignLeft);
	}

	// Mass
	{
		WidgetDoubleSpinBox* spinMass = setSpinBox("Mass", pBody->mMass, widLayout, 2, 0);
		connect(spinMass, &WidgetDoubleSpinBox::valueChanged, this, [spinMass, pBody]
		{
			pBody->setMass(spinMass->value());
		});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::createScript()
{
	Game::Component::CPScript* newScript = new Game::Component::CPScript();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Script, newScript);

	addScript(newScript);
}

void WidgetInspectorData::addScript(Game::Component::CPScript* pScript)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pScript, Game::Utils::ComponentType::Script, QIcon(mSetting.value("MenuInspectorScriptingIcon").toString()), mSetting.value("MenuInspectorScripting").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setVerticalSpacing(2);

	// Script name
	mScriptName = new QLabel(pScript->mName.c_str());
	widLayout->addWidget(mScriptName);

	// Script path
	mScriptPath = new WidgetDropInput(*mScriptName, *pScript, Tools::Utils::PathParser::EFileType::SCRIPT);
	mScriptPath->setText(pScript->mPath.c_str());
	mScriptPath->setReadOnly(true);
	widLayout->addWidget(mScriptPath);
	connect(mScriptPath, &WidgetDropInput::textChanged, this, [=]
	{
		pScript->setScript(nullptr);
	});

	const QIcon folderIcon(mSetting.value("FolderIcon").toString());
	QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
	mScriptPath->addAction(browseAction, QLineEdit::TrailingPosition);
	connect(browseAction, &QAction::triggered, this, [=]
	{
		setScript(pScript, mScriptName, mScriptPath);
	});
	service(Scripting::ScriptInterpreter).mBehaviours.push_back(pScript); //put script into interpreter

	QWidget* container = new QWidget();
	container->setObjectName("Component");
	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(15, 5, 5, 5);
	grid->setVerticalSpacing(1);
	container->setLayout(grid);
	widLayout->addWidget(container);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pScript->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		grid->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pScript->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::setScript(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	std::string filter = "Script File (*.lua)";

	QFileDialog dialog(this, tr("Open Script"), Defaultpath, tr(filter.c_str()));
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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPScript*)pComponent)->setScript(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::setScript(QFileInfo& pInfo)
{
	Game::Component::CPScript* newScript = new Game::Component::CPScript();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Script, newScript);

	addScript(newScript);

	mScriptName->setText(pInfo.baseName());
	mScriptPath->setText(pInfo.absoluteFilePath());
	newScript->setScript(Utils::qStringToStdString(pInfo.baseName())
		, Utils::qStringToStdString(pInfo.absoluteFilePath()).c_str());
}

void WidgetInspectorData::createSound()
{
	Game::Component::CPSound* newSound = new Game::Component::CPSound();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Sound, newSound);

	addSound(newSound);
}

void WidgetInspectorData::addSound(Game::Component::CPSound* pSound)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pSound, Game::Utils::ComponentType::Sound, QIcon(mSetting.value("MenuInspectorSoundIcon").toString()), mSetting.value("MenuInspectorSound").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QVBoxLayout* vertical = new QVBoxLayout();

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setVerticalSpacing(2);

	// Sound name
	mSoundName = new QLabel(pSound->mName.c_str());
	vertical->addWidget(mSoundName);

	// Sound path
	mSoundPath = new WidgetDropInput(*mSoundName, *pSound, Tools::Utils::PathParser::EFileType::SOUND);
	mSoundPath->setText(pSound->mPath.c_str());
	mSoundPath->setReadOnly(true);
	vertical->addWidget(mSoundPath);
	connect(mSoundPath, &WidgetDropInput::textChanged, this, [=]
	{
		pSound->setClip(nullptr);
	});

	QWidget* bottom = new QWidget();
	bottom->setObjectName("Component");
	bottom->setLayout(widLayout);
	vertical->addWidget(bottom);

	const QIcon folderIcon(mSetting.value("FolderIcon").toString());
	QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
	mSoundPath->addAction(browseAction, QLineEdit::TrailingPosition);
	connect(browseAction, &QAction::triggered, this, [=]
	{
		setSound(pSound, mSoundName, mSoundPath);
	});

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSound->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 1, 1, Qt::AlignRight);
		widLayout->addWidget(box, 1, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pSound->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSound->mute ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Mute"), 2, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 2, 1, Qt::AlignRight);
		widLayout->addWidget(box, 2, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
		{
			box->clearFocus();
			pSound->mute = box->checkState() == Qt::CheckState::Checked;
		});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSound->playOnAwake ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Play On Awake"), 3, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 3, 1, Qt::AlignRight);
		widLayout->addWidget(box, 3, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
		{
			box->clearFocus();
			pSound->playOnAwake = box->checkState() == Qt::CheckState::Checked;
		});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSound->loop ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Loop"), 4, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 4, 1, Qt::AlignRight);
		widLayout->addWidget(box, 4, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
		{
			box->clearFocus();
			pSound->loop = box->checkState() == Qt::CheckState::Checked;
		});
	}

	// QSlider
	{
		QLabel* label = new QLabel(QString::number(pSound->volume));

		QSlider* slider = new QSlider(Qt::Orientation::Horizontal);
		slider->setMinimum(0);
		slider->setMaximum(100);
		slider->setValue(pSound->volume);

		connect(slider, &QSlider::valueChanged, this, [=]
		{
			label->setText(QString::number(slider->value()));

			pSound->volume = slider->value();
			slider->clearFocus();
		});

		widLayout->addWidget(new QLabel("Volume"), 5, 0, Qt::AlignLeft);
		widLayout->addWidget(label, 5, 1, Qt::AlignRight);
		widLayout->addWidget(slider, 5, 2, 1, 4, Qt::AlignLeft);
	}
	
	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSound->is3DSound ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("3D Sound"), 6, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 6, 1, Qt::AlignRight);
		widLayout->addWidget(box, 6, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
		{
			box->clearFocus();
			pSound->is3DSound = box->checkState() == Qt::CheckState::Checked;
		});
	}

	// Min Distance
	{
		WidgetDoubleSpinBox* spinMass = setSpinBox("Min Distance", pSound->minDist, widLayout, 7, 0);
		connect(spinMass, &WidgetDoubleSpinBox::valueChanged, this, [spinMass, pSound]
		{
			pSound->minDist = spinMass->value();
		});
	}

	wiz->setContentLayout(*vertical);
}

void WidgetInspectorData::setSound(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	std::string filter = "Sound File (*.mp3)";

	QFileDialog dialog(this, tr("Open Sound"), Defaultpath, tr(filter.c_str()));
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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPSound*)pComponent)->setClip(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::setSound(QFileInfo& pInfo)
{
	Game::Component::CPSound* newSound = new Game::Component::CPSound();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Sound, newSound);

	addSound(newSound);

	mSoundName->setText(pInfo.baseName());
	mSoundPath->setText(pInfo.absoluteFilePath());
	newSound->setClip(Utils::qStringToStdString(pInfo.baseName())
		, Utils::qStringToStdString(pInfo.absoluteFilePath()).c_str());
}

void WidgetInspectorData::createSoundListener()
{
	Game::Component::CPSoundListener* newSound = new Game::Component::CPSoundListener();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::SoundListener, newSound);

	addSoundListener(newSound);
}

void WidgetInspectorData::addSoundListener(Game::Component::CPSoundListener* pSound)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pSound, Game::Utils::ComponentType::SoundListener, QIcon(mSetting.value("MenuInspectorSoundListenerIcon").toString()), mSetting.value("MenuInspectorSoundListener").toString(), false);
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setContentsMargins(15, 5, 5, 5);
	widLayout->setVerticalSpacing(1);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSound->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pSound->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::createUI()
{
	Game::Component::CPUI* newUI = new Game::Component::CPUI();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::UI, newUI);

	addUI(newUI);
}

void WidgetInspectorData::addUI(Game::Component::CPUI* pUI)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pUI, Game::Utils::ComponentType::UI, QIcon(mSetting.value("MenuInspectorUIIcon").toString()), mSetting.value("MenuInspectorUI").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setVerticalSpacing(2);

	// UI name
	mUIName = new QLabel(pUI->mNameUI.c_str());
	widLayout->addWidget(mUIName);

	// UI path
	mUIPath = new WidgetDropInput(*mUIName, *pUI, Tools::Utils::PathParser::EFileType::UI);
	mUIPath->setText(pUI->mPathUI.c_str());
	mUIPath->setReadOnly(true);
	widLayout->addWidget(mUIPath);
	connect(mUIPath, &WidgetDropInput::textChanged, this, [=]
	{
		pUI->setUIWithPath(nullptr);
	});

	const QIcon folderIcon(mSetting.value("FolderIcon").toString());
	QAction* browseAction = new QAction(folderIcon, mSetting.value("FolderTip").toString());
	mUIPath->addAction(browseAction, QLineEdit::TrailingPosition);
	connect(browseAction, &QAction::triggered, this, [=]
	{
		setUI(pUI, mUIName, mUIPath);
	});

	QWidget* container = new QWidget();
	container->setObjectName("Component");
	QGridLayout* grid = new QGridLayout();
	grid->setContentsMargins(15, 5, 5, 5);
	grid->setVerticalSpacing(1);
	container->setLayout(grid);
	widLayout->addWidget(container);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pUI->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		grid->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		grid->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		grid->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pUI->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}
	
	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::setUI(Game::Component::AComponent* pComponent, QLabel* name, QLineEdit* mProjectPath)
{
	QString Defaultpath = service(Data::ProjectLocation).mFolder;

	std::string filter = "UI File (*.ux)";

	QFileDialog dialog(this, tr("Open UI"), Defaultpath, tr(filter.c_str()));
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

		name->setText(info.baseName());
		mProjectPath->setText(fileName);
		((Game::Component::CPUI*)pComponent)->setUI(Utils::qStringToStdString(info.baseName())
			, Utils::qStringToStdString(info.absoluteFilePath()).c_str());
	}
}

void WidgetInspectorData::setUI(QFileInfo& pInfo)
{
	Game::Component::CPUI* newUI = new Game::Component::CPUI();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::UI, newUI);

	addUI(newUI);

	mUIName->setText(pInfo.baseName());
	mUIPath->setText(pInfo.absoluteFilePath());
	newUI->setUI(Utils::qStringToStdString(pInfo.baseName())
		, Utils::qStringToStdString(pInfo.absoluteFilePath()).c_str());
}

void WidgetInspectorData::createDirLight()
{
	Game::Component::CPDirLight* newDirLight = new Game::Component::CPDirLight();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Light, newDirLight);

	addDirLight(newDirLight);
}

void WidgetInspectorData::addDirLight(Game::Component::CPDirLight* pDirLight)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pDirLight, Game::Utils::ComponentType::Light, QIcon(mSetting.value("MenuInspectorDirLightIcon").toString()), mSetting.value("MenuInspectorDirLight").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setVerticalSpacing(2);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pDirLight->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pDirLight->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	//Color
	{
		QPushButton* color = new QPushButton();
		color->setSizePolicy(QSizePolicy::MinimumExpanding,
								QSizePolicy::MinimumExpanding);
		QColor* aColor = new QColor(pDirLight->mColor.x * 255, pDirLight->mColor.y * 255, pDirLight->mColor.z * 255);
		QString qss = QString("background-color: %1").arg(aColor->name());
		color->setStyleSheet(qss);

		widLayout->addWidget(new QLabel("Light Color"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(color, 1, 1, 1, 4);
		connect(color, &QPushButton::pressed, this, [this, pDirLight, aColor, color]
		{
			*aColor = setColorWidget(*aColor);

			QString qss = QString("background-color: %1").arg(aColor->name());
			color->setStyleSheet(qss);

			pDirLight->mColor = Maths::FVector3(aColor->red() / 255.f, aColor->green() / 255.f, aColor->blue() / 255.f);
		});
	}

	//Ateniation
	{
		widLayout->addWidget(new QLabel("Attenuatuion"), 2, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 2, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pDirLight->mAttenuatuion, 0, 1, 0.1f);
		widLayout->addWidget(spinBox, 2, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pDirLight]
		{
			pDirLight->mAttenuatuion  = spinBox->value();
		});
	}

	//Brightness
	{
		widLayout->addWidget(new QLabel("Max Brightness"), 3, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 3, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pDirLight->mBrightness, 0, INT64_MAX);
		widLayout->addWidget(spinBox, 3, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pDirLight]
		{
			pDirLight->mBrightness = spinBox->value();
		});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pDirLight->mCastShadow ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Cast Shadow"), 4, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 4, 1, Qt::AlignRight);
		widLayout->addWidget(box, 4, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [pDirLight, box]
		{
			box->clearFocus();
			pDirLight->mCastShadow = box->checkState() == Qt::CheckState::Checked;
		});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::createPointLight()
{
	Game::Component::CPPointLight* newPointLight = new Game::Component::CPPointLight();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Light, newPointLight);

	addPointLight(newPointLight);
}

void WidgetInspectorData::addPointLight(Game::Component::CPPointLight* pPointLight)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pPointLight, Game::Utils::ComponentType::Light, QIcon(mSetting.value("MenuInspectorPointLightIcon").toString()), mSetting.value("MenuInspectorPointLight").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setVerticalSpacing(2);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pPointLight->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pPointLight->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	//Color
	{
		QPushButton* color = new QPushButton();
		color->setSizePolicy(QSizePolicy::MinimumExpanding,
			QSizePolicy::MinimumExpanding);
		QColor* aColor = new QColor(pPointLight->mColor.x * 255, pPointLight->mColor.y * 255, pPointLight->mColor.z * 255);
		QString qss = QString("background-color: %1").arg(aColor->name());
		color->setStyleSheet(qss);

		widLayout->addWidget(new QLabel("Light Color"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(color, 1, 1, 1, 4);
		connect(color, &QPushButton::pressed, this, [this, pPointLight, aColor, color]
		{
			*aColor = setColorWidget(*aColor);

			QString qss = QString("background-color: %1").arg(aColor->name());
			color->setStyleSheet(qss);

			pPointLight->mColor = Maths::FVector3(aColor->red() / 255.f, aColor->green() / 255.f, aColor->blue() / 255.f);
		});
	}

	//Ateniation
	{
		widLayout->addWidget(new QLabel("Attenuatuion"), 2, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 2, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pPointLight->mAttenuatuion, 0, 1, 0.1f);
		widLayout->addWidget(spinBox, 2, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pPointLight]
		{
			pPointLight->mAttenuatuion = spinBox->value();
		});
	}

	//Radius
	{
		widLayout->addWidget(new QLabel("Radius"), 3, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 3, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pPointLight->mRadius, 0, INT64_MAX);
		widLayout->addWidget(spinBox, 3, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pPointLight]
		{
			pPointLight->mRadius = spinBox->value();
#ifdef NSHIPPING
			pPointLight->updateDrawer();
#endif
		});
	}
	
	//Brightness
	{
		widLayout->addWidget(new QLabel("Max Brightness"), 4, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 4, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pPointLight->mBrightness, 0, INT64_MAX);
		widLayout->addWidget(spinBox, 4, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pPointLight]
		{
			pPointLight->mBrightness = spinBox->value();
		});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pPointLight->mCastShadow ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Cast Shadow"), 5, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 5, 1, Qt::AlignRight);
		widLayout->addWidget(box, 5, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [pPointLight, box]
		{
			box->clearFocus();
			pPointLight->mCastShadow = box->checkState() == Qt::CheckState::Checked;
		});
	}

	wiz->setContentLayout(*widLayout);
}

void WidgetInspectorData::createSpotLight()
{
	Game::Component::CPSpotLight* newSpotLight = new Game::Component::CPSpotLight();
	((WidgetGameObjectTreeItem*)mItem)->mActor->addComponent(Game::Utils::ComponentType::Light, newSpotLight);

	addSpotLight(newSpotLight);
}

void WidgetInspectorData::addSpotLight(Game::Component::CPSpotLight* pSpotLight)
{
	WidgetCollapse* wiz = new WidgetCollapse(mSetting, *((WidgetGameObjectTreeItem*)mItem)->mActor, *pSpotLight, Game::Utils::ComponentType::Light, QIcon(mSetting.value("MenuInspectorSpotLightIcon").toString()), mSetting.value("MenuInspectorSpotLight").toString());
	mLayout->insertWidget(mLayout->count() - 1, wiz, 0, Qt::AlignTop);
	wiz->setObjectName("Component");

	QGridLayout* widLayout = new QGridLayout();
	widLayout->setVerticalSpacing(2);

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSpotLight->isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Active"), 0, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 0, 1, Qt::AlignRight);
		widLayout->addWidget(box, 0, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [=]
			{
				box->clearFocus();
				pSpotLight->isActive = box->checkState() == Qt::CheckState::Checked;
			});
	}

	//Color
	{
		QPushButton* color = new QPushButton();
		color->setSizePolicy(QSizePolicy::MinimumExpanding,
			QSizePolicy::MinimumExpanding);
		QColor* aColor = new QColor(pSpotLight->mColor.x * 255, pSpotLight->mColor.y * 255, pSpotLight->mColor.z * 255);
		QString qss = QString("background-color: %1").arg(aColor->name());
		color->setStyleSheet(qss);

		widLayout->addWidget(new QLabel("Light Color"), 1, 0, Qt::AlignLeft);
		widLayout->addWidget(color, 1, 1, 1, 4);
		connect(color, &QPushButton::pressed, this, [this, pSpotLight, aColor, color]
		{
			*aColor = setColorWidget(*aColor);

			QString qss = QString("background-color: %1").arg(aColor->name());
			color->setStyleSheet(qss);

			pSpotLight->mColor = Maths::FVector3(aColor->red() / 255.f, aColor->green() / 255.f, aColor->blue() / 255.f);
		});
	}

	//Ateniation
	{
		widLayout->addWidget(new QLabel("Attenuatuion"), 2, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 2, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pSpotLight->mAttenuatuion, 0, 1, 0.1f);
		widLayout->addWidget(spinBox, 2, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pSpotLight]
		{
			pSpotLight->mAttenuatuion = spinBox->value();
		});
	}

	//CutOff
	{
		widLayout->addWidget(new QLabel("CutOff"), 3, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 3, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pSpotLight->mCutOff, 0, 360);
		widLayout->addWidget(spinBox, 3, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pSpotLight]
		{
			pSpotLight->mCutOff = spinBox->value();
			pSpotLight->mCutOffRad = std::cosf(Maths::degreesToRadians(pSpotLight->mCutOff));
		});
	}

	//OuterCutOff
	{
		widLayout->addWidget(new QLabel("OuterCutOff"), 4, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 4, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pSpotLight->mOuterCutOff, 0, 360);
		widLayout->addWidget(spinBox, 4, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pSpotLight]
		{
			pSpotLight->mOuterCutOff = spinBox->value();
			pSpotLight->mOuterCutOffRad = std::cosf(Maths::degreesToRadians(pSpotLight->mOuterCutOff));
		});
	}

	//Brightness
	{
		widLayout->addWidget(new QLabel("Max Brightness"), 5, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 5, 1, Qt::AlignRight);

		WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pSpotLight->mBrightness, 0, INT64_MAX);
		widLayout->addWidget(spinBox, 5, 2, 1, 4, Qt::AlignLeft);
		connect(spinBox, &WidgetDoubleSpinBox::valueChanged, this, [spinBox, pSpotLight]
		{
			pSpotLight->mBrightness = spinBox->value();
		});
	}

	// check box
	{
		QCheckBox* box = new QCheckBox();
		box->setCheckState(pSpotLight->mCastShadow ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
		widLayout->addWidget(new QLabel("Cast Shadow"), 6, 0, Qt::AlignLeft);
		widLayout->addWidget(new QLabel(" "), 6, 1, Qt::AlignRight);
		widLayout->addWidget(box, 6, 2, 1, 4, Qt::AlignLeft);
		connect(box, &QCheckBox::stateChanged, this, [pSpotLight, box]
			{
				box->clearFocus();
				pSpotLight->mCastShadow = box->checkState() == Qt::CheckState::Checked;
			});
	}

	wiz->setContentLayout(*widLayout);
}

WidgetDoubleSpinBox* WidgetInspectorData::setSpinBox(std::string pText, float& pAxisVal, QGridLayout* pLayout, int row, int column)
{
	pLayout->addWidget(new QLabel(pText.c_str()), row, column, Qt::AlignLeft);
	pLayout->addWidget(new QLabel(" "), row, column + 1, Qt::AlignRight);

	WidgetDoubleSpinBox* spinBox = new WidgetDoubleSpinBox(pAxisVal, 0, INT64_MAX);
	pLayout->addWidget(spinBox, row, column + 2, 1, 4, Qt::AlignLeft);

	return spinBox;
}

void WidgetInspectorData::setIndividualBoxAxisScrollbar(std::string pText, float& pAxisVal, QGridLayout* pLayout, Game::Component::ACollider& pCol, int row, int column)
{
	pLayout->addWidget(new QLabel(pText.c_str()), row, column, Qt::AlignRight);

	WidgetDoubleSpinBox* SpinBox = new WidgetDoubleSpinBox(pAxisVal, INT64_MIN, INT64_MAX);
	pLayout->addWidget(SpinBox, row, column + 1, Qt::AlignLeft);
	connect(SpinBox, &WidgetDoubleSpinBox::valueChanged, this, [SpinBox, &pAxisVal, &pCol, this]
		{
			pAxisVal = SpinBox->value();
			pCol.UpdateTransform(*((WidgetGameObjectTreeItem*)mItem)->mActor->getTransform());
		});
}

void WidgetInspectorData::setIndividualBoxAxisScrollbar(std::string pText, float& pAxisVal, QGridLayout* pLayout, Game::Component::CPDecal& pCol, int row, int column)
{
	pLayout->addWidget(new QLabel(pText.c_str()), row, column, Qt::AlignRight);

	WidgetDoubleSpinBox* SpinBox = new WidgetDoubleSpinBox(pAxisVal, INT64_MIN, INT64_MAX);
	pLayout->addWidget(SpinBox, row, column + 1, Qt::AlignLeft);
	connect(SpinBox, &WidgetDoubleSpinBox::valueChanged, this, [SpinBox, &pAxisVal, &pCol, this]
	{
		pAxisVal = SpinBox->value();
			
		//Recreate Box
#ifdef NSHIPPING
		pCol.updateDrawer();
#endif
	});
}

QColor WidgetInspectorData::setColorWidget(const QColor& pColor)
{
	Editor::Widget::WidgetColor dialog(pColor);
	if (dialog.exec() == 0)
		return dialog.currentColor();
	return pColor;
}