#include "Editor/Widget/WidgetMaterialApp.h"
#include <QtWidgets/qboxlayout.h>
#include <QtNodes/ConnectionStyle>
#include "Editor/Nodes/ColorPicker.h"
#include "Editor/Nodes/TexturePicker.h"
#include "Editor/Nodes/MaterialResult.h"
#include <QtWidgets/qpushbutton.h>
#include <QtGui/qicon.h>
#include <QtCore/qjsonarray.h>
#include <EngineCore/ResourceManagement/ResourceManager.h>
#include <EngineCore/Service/ServiceLocator.h>
#include <Rendering/Resources/Material.h>
#include "Editor/Utils/Utils.h"

using namespace Editor::Widget;

WidgetMaterialApp::WidgetMaterialApp(QSettings& pSetting, QWidget* pParent) :
	ads::CDockWidget("Material"), mModel(registerDataModels())
{
	QWidget* content = new QWidget(nullptr);
	setWidget(content);

	mLayout->setContentsMargins(5, 0, 5, 5);
	mLayout->setSpacing(0);
	content->setLayout(mLayout);

    setStyle();
    mScene = new QtNodes::DataFlowGraphicsScene(mModel, content);
    mView = new QtNodes::GraphicsView(mScene);
    
    mLayout->addWidget(mView);

    content->setAcceptDrops(true);
    setAcceptDrops(true);
    mView->viewport()->setAcceptDrops(true);
    mView->setAcceptDrops(true);

    installEventFilter(this);

    QPushButton* button = new QPushButton(QIcon(pSetting.value("MenuFileSaveMapIcon").toString()), "", content);
    connect(button, &QPushButton::pressed, this, &WidgetMaterialApp::saveMaterial);
}

void WidgetMaterialApp::setStyle()
{
    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
          {
            "ConnectionStyle": {
              "UseDataDefinedColors": true
            }
          }
          )");
}

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> WidgetMaterialApp::registerDataModels()
{
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    
    ret->registerModel<Nodes::ColorPicker>("Color");
    ret->registerModel<Nodes::TexturePicker>("Texture");
    ret->registerModel<Nodes::MaterialResult>("Result");

    return ret;
}

bool WidgetMaterialApp::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneDrop || event->type() == QEvent::Drop)
        return true;

    return ads::CDockWidget::eventFilter(object, event);
}

void WidgetMaterialApp::dragEnterEvent(QDragEnterEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void WidgetMaterialApp::dragMoveEvent(QDragMoveEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void WidgetMaterialApp::dropEvent(QDropEvent* pEvent)
{
    bool a = true;

}

void WidgetMaterialApp::saveMaterial()
{
    if (mCurrentMat.isEmpty())
        return;

    QFile file(mCurrentMat);
    if (file.open(QIODevice::WriteOnly)) 
    {
        QJsonObject res;
        for (auto const nodeId : mModel.allNodeIds()) 
        {
            if (mModel._models[nodeId].get()->name() == "MaterialResult")
            {
                res = ((Editor::Nodes::MaterialResult*)mModel._models[nodeId].get())->saveFinalResult();
                break;
            }
        }

        QJsonObject obj = mModel.save();
        obj.insert("Result", res);
        
        QJsonDocument doc(obj);
        file.write(doc.toJson());
        file.close();

        std::string str = Editor::Utils::qStringToStdString(mCurrentMat);
        service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Material>(str.c_str(), str)->update();
    }
}

void WidgetMaterialApp::open(QString pPath)
{
    toggleView(true);
    mScene->clearScene();

    QFile file(pPath);

    if (!file.open(QIODevice::ReadOnly))
        return;

    mCurrentMat = pPath;
    
    QByteArray const wholeFile = file.readAll();

    mModel.load(QJsonDocument::fromJson(wholeFile).object());

    Q_EMIT mScene->sceneLoaded();
}

void WidgetMaterialApp::close()
{
    saveMaterial();
    toggleView(false);
}