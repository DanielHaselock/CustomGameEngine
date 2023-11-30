#include <QtWidgets/qboxlayout.h>
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/Thread/ThreadPool.h"
#include "Editor/Widget/WidgetModelViewerApp.h"
#include "Maths/Utils.h"
#include "Maths/FQuaternion.h"
#include "Rendering/Resources/VK/PipeLineBuilder.h"
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "EngineCore/EventSystem/InputManager.h"
#include <QtCore/qfileinfo.h>
#include "Tools/Utils/PathParser.h"

using namespace Editor::Widget;

WidgetModelViewerApp::WidgetModelViewerApp(QSettings& pSetting, QWidget* pParent) :
	ads::CDockWidget("Asset View"), mWindow("", 800, 800), mCamera(mWindow.mWidth, mWindow.mHeight),
	mUnibuffer(VK_SHADER_STAGE_VERTEX_BIT), mGridUnibuffer(VK_SHADER_STAGE_VERTEX_BIT),
	mDefaultTexture("Resources/Engine/Textures/default.png")
{
	renderer.init(mWindow);

	mUnibuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix({}, { }, { Maths::FVector3::One });

	mDefaultMat = new  Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Editor/Shaders/AssetViewVertex.vert.spv", "Resources/Editor/Shaders/AssetViewFrag.frag.spv", renderer.mRenderPass, false));
	mDefaultMat->bindDescriptor("ubo", mUnibuffer.mDescriptorSets);
	mDefaultMat->bindDescriptor("texAlbedo", mDefaultTexture.mTextureSets);

	mGridMat = new  Rendering::Data::Material(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Editor/Shaders/gridVertex.vert.spv", "Resources/Editor/Shaders/gridFrag.frag.spv", renderer.mRenderPass, true));
	mGridMat->bindDescriptor("ubo", mGridUnibuffer.mDescriptorSets);

	QWidget* content = new QWidget(pParent);
	setWidget(content);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, nullptr);
	layout->setContentsMargins(5, 0, 5, 5);
	layout->setSpacing(0);
	content->setLayout(layout);

	setAcceptDrops(true);

	windowId = mWindow.getWindowId();
	qWindow = QWindow::fromWinId((WId)windowId);

	renderingWindow = QWidget::createWindowContainer(qWindow);

	renderingWindow->installEventFilter(this);
	layout->addWidget(renderingWindow);
}

WidgetModelViewerApp::~WidgetModelViewerApp()
{
	delete mDefaultMat;
	delete mGridMat;
}

void WidgetModelViewerApp::open(Rendering::Resources::Model* pModel)
{
	if (pModel == nullptr)
	{
		QMessageBox msgBox;
		msgBox.setText("Not finish loading Model");
		msgBox.exec();
		return;
	}

	mModel = pModel;
	toggleView(true);

	if (!mIsActive)
		run();
}

void WidgetModelViewerApp::run()
{
	service(EngineCore::Thread::ThreadPool).queueJob([=]
		{
			mIsActive = true;

			Rendering::LineDrawer lines(renderer);

			EngineCore::EventSystem::InputManager input;
			mWindow.setInputManager(&input);

			Rendering::Resources::Model* oldModel = nullptr;
			while (!isClosed())
			{
				mWindow.pollEvents();

				if (oldModel != mModel)
				{
					oldModel = mModel;
					updateModelTransform();
					createBoundingBox(lines);
				}

				if (renderer.mFramebufferResized)
				{
					int width, height;
					mWindow.getFramebufferSize(&width, &height);
					mCamera.updateProjection(width, height);

					renderer.mFramebufferResized = false;
				}

				input.processInput();

				Maths::FVector2 dif = input.mMouse.getPositionDifference();
				if (input.mMouse.isLeftPressed())
					mCamera.update(dif);

				// Zoom in and out with the mouse wheel
				// The mouse wheel callback return 1 or -1
				if (input.mMouse.isScrolling())
				{
					mCamera.zoom(input.mMouse.mScroll);
					input.mMouse.mScroll = 0;
				}

				mGridUnibuffer.mData.mView = mCamera.lookAt();
				mGridUnibuffer.mData.mProjection = mCamera.mProjection;
				mGridUnibuffer.updateData();

				mUnibuffer.mData.mViewProjection = mCamera.viewProj();
				mUnibuffer.updateData();

				lines.updateViewProj(mCamera.viewProj());

				renderer.beginFrame();
				renderer.beginWorldPass();
				//Model
				mDefaultMat->bindPipeLine(renderer.getCurrentCommandBuffer());
				if (mModel != nullptr)
					mModel->draw(renderer.getCurrentCommandBuffer());

				//Line
				lines.flushLines();

				//Grid
				mGridMat->bindPipeLine(renderer.getCurrentCommandBuffer());
				vkCmdDraw(renderer.getCurrentCommandBuffer(), 6, 1, 0, 0);
				renderer.endWorldPass();
				renderer.endFrame();
			}

			renderer.waitForCleanUp();

			mIsActive = false;
		});
}

void WidgetModelViewerApp::createBoundingBox(Rendering::LineDrawer& pLinesDrawer)
{
	pLinesDrawer.reset();

	if (mModel == nullptr)
		return;
	
	pLinesDrawer.drawLine(mModel->mBox.mMin, { mModel->mBox.mMin.x, mModel->mBox.mMin.y, mModel->mBox.mMax.z }, { 0, 1, 0 });
	pLinesDrawer.drawLine(mModel->mBox.mMin, { mModel->mBox.mMin.x, mModel->mBox.mMax.y, mModel->mBox.mMin.z }, { 0, 1, 0 });
	pLinesDrawer.drawLine(mModel->mBox.mMin, { mModel->mBox.mMax.x, mModel->mBox.mMin.y, mModel->mBox.mMin.z }, { 0, 1, 0 });

	pLinesDrawer.drawLine(mModel->mBox.mMax, { mModel->mBox.mMax.x, mModel->mBox.mMax.y, mModel->mBox.mMin.z }, { 0, 1, 0 });
	pLinesDrawer.drawLine(mModel->mBox.mMax, { mModel->mBox.mMax.x, mModel->mBox.mMin.y, mModel->mBox.mMax.z }, { 0, 1, 0 });
	pLinesDrawer.drawLine(mModel->mBox.mMax, { mModel->mBox.mMin.x, mModel->mBox.mMax.y, mModel->mBox.mMax.z }, { 0, 1, 0 });

	pLinesDrawer.drawLine({ mModel->mBox.mMin.x, mModel->mBox.mMax.y, mModel->mBox.mMax.z }, { mModel->mBox.mMin.x, mModel->mBox.mMax.y, mModel->mBox.mMin.z }, { 0, 1, 0 });
	pLinesDrawer.drawLine({ mModel->mBox.mMax.x, mModel->mBox.mMax.y, mModel->mBox.mMin.z }, { mModel->mBox.mMin.x, mModel->mBox.mMax.y, mModel->mBox.mMin.z }, { 0, 1, 0 });

	pLinesDrawer.drawLine({ mModel->mBox.mMin.x, mModel->mBox.mMax.y, mModel->mBox.mMax.z }, { mModel->mBox.mMin.x, mModel->mBox.mMin.y, mModel->mBox.mMax.z }, { 0, 1, 0 });
	pLinesDrawer.drawLine({ mModel->mBox.mMax.x, mModel->mBox.mMax.y, mModel->mBox.mMin.z }, { mModel->mBox.mMax.x, mModel->mBox.mMin.y, mModel->mBox.mMin.z }, { 0, 1, 0 });

	pLinesDrawer.drawLine({ mModel->mBox.mMax.x, mModel->mBox.mMin.y, mModel->mBox.mMax.z }, { mModel->mBox.mMin.x, mModel->mBox.mMin.y, mModel->mBox.mMax.z }, { 0, 1, 0 });
	pLinesDrawer.drawLine({ mModel->mBox.mMax.x, mModel->mBox.mMin.y, mModel->mBox.mMax.z }, { mModel->mBox.mMax.x, mModel->mBox.mMin.y, mModel->mBox.mMin.z }, { 0, 1, 0 });

	pLinesDrawer.mUniformBuffer.mData.mModel = mUnibuffer.mData.mModel;
}

void WidgetModelViewerApp::updateModelTransform()
{
	float max = Maths::fmax(Maths::fmax(mModel->mBox.mSize.x, mModel->mBox.mSize.y), mModel->mBox.mSize.z);
	float dist = max / tan(Maths::degreesToRadians(45) / 2);
	Maths::FVector3 scale = { 2 / dist, 2 / dist, 2 / dist };

	float height = mModel->mBox.mMin.y + (mModel->mBox.mSize.y / 2);
	float halfHeight = (mModel->mBox.mSize.y / 2) * scale.y;
	if ((height + EPSILON >= 0) && (height - EPSILON <= 0))
		mUnibuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix({ 0, halfHeight, 0 }, { }, scale);
	else
		mUnibuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix({}, {}, scale);

	mCamera.updateHeight(halfHeight);
}

void WidgetModelViewerApp::close()
{
	toggleView(false);
}

bool WidgetModelViewerApp::eventFilter(QObject* obj, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::DragEnter)
		((QDragEnterEvent*)pEvent)->acceptProposedAction();
	else if (pEvent->type() == QEvent::Drop)
	{
		QFileInfo info = QFileInfo(((QDropEvent*)pEvent)->mimeData()->text());
		if (Utils::getFileType(info.suffix()) != Tools::Utils::PathParser::EFileType::MODEL)
			return QObject::eventFilter(obj, pEvent);

		std::string path = Utils::qStringToStdString(info.absoluteFilePath());
		open(service(EngineCore::ResourcesManagement::ResourceManager).get<Rendering::Resources::Model>(path));
	}

	return QObject::eventFilter(obj, pEvent);
}