#include "Editor/Widget/WidgetParticleApp.h"
#include <QtWidgets/qboxlayout.h>
#include <QtNodes/ConnectionStyle>
#include "Editor/Nodes/ColorPicker.h"
#include "Editor/Nodes/TexturePicker.h"
#include "Editor/Nodes/ParticleResult.h"
#include "Editor/Nodes/Vec3Picker.h"
#include "Editor/Nodes/UnsignedVec3Picker.h"
#include "Editor/Nodes/FloatPicker.h"
#include "Editor/Nodes/UnsignedFloatPicker.h"
#include "Editor/Nodes/IntegerPicker.h"
#include "Editor/Nodes/UnsignedIntegerPicker.h"
#include <QtWidgets/qpushbutton.h>
#include <QtGui/qicon.h>
#include <QtCore/qjsonarray.h>
#include <EngineCore/ResourceManagement/ResourceManager.h>
#include <EngineCore/Service/ServiceLocator.h>
#include <Rendering/Resources/Particle.h>
#include "Editor/Utils/Utils.h"

using namespace Editor::Widget;

WidgetParticleApp::WidgetParticleApp(QSettings& pSetting, QWidget* pParent) :
	ads::CDockWidget("Particle"), mModel(registerDataModels())
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
    connect(button, &QPushButton::pressed, this, &WidgetParticleApp::saveParticle);
}

void WidgetParticleApp::setStyle()
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

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> WidgetParticleApp::registerDataModels()
{
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    
    ret->registerModel<Nodes::FloatPicker>("Single value");
    ret->registerModel<Nodes::UnsignedFloatPicker>("Single value");
    ret->registerModel<Nodes::IntegerPicker>("Single value");
    ret->registerModel<Nodes::UnsignedIntegerPicker>("Single value");

    ret->registerModel<Nodes::Vec3Picker>("Vector");
    ret->registerModel<Nodes::UnsignedVec3Picker>("Vector");
    ret->registerModel<Nodes::ColorPicker>("Vector");
    ret->registerModel<Nodes::TexturePicker>("Vector");

    ret->registerModel<Nodes::ParticleResult>("Result");

    return ret;
}

bool WidgetParticleApp::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneDrop || event->type() == QEvent::Drop)
        return true;

    return ads::CDockWidget::eventFilter(object, event);
}

void WidgetParticleApp::dragEnterEvent(QDragEnterEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void WidgetParticleApp::dragMoveEvent(QDragMoveEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void WidgetParticleApp::dropEvent(QDropEvent* pEvent)
{
    bool a = true;

}

void WidgetParticleApp::saveParticle()
{
    if (mCurrentPath.isEmpty())
        return;

    QFile file(mCurrentPath);
    if (file.open(QIODevice::WriteOnly)) 
    {
        QJsonObject res;
        for (auto const nodeId : mModel.allNodeIds()) 
        {
            if (mModel._models[nodeId].get()->name() == "ParticleResult")
            {
                res = ((Editor::Nodes::ParticleResult*)mModel._models[nodeId].get())->saveFinalResult();
                break;
            }
        }

        QJsonObject obj = mModel.save();
        obj.insert("Result", res);
        
        QJsonDocument doc(obj);
        file.write(doc.toJson());
        file.close();

        std::string str = Editor::Utils::qStringToStdString(mCurrentPath);
        service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::ParticleSystem>(str.c_str(), str)->parseFile();
    }
}

void WidgetParticleApp::open(QString pPath)
{
    toggleView(true);
    mScene->clearScene();

    QFile file(pPath);

    if (!file.open(QIODevice::ReadOnly))
        return;

    mCurrentPath = pPath;
    
    QByteArray const wholeFile = file.readAll();

    mModel.load(QJsonDocument::fromJson(wholeFile).object());

    Q_EMIT mScene->sceneLoaded();
}

void WidgetParticleApp::close()
{
    saveParticle();
    toggleView(false);
}