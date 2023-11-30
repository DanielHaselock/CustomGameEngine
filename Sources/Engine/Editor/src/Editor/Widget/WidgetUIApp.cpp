#include "Editor/Widget/WidgetUIApp.h"
#include <QtWidgets/qboxlayout.h>
#include <QtNodes/ConnectionStyle>

#include "Editor/UI/Button.h"
#include "Editor/UI/Image.h"
#include "Editor/UI/Text.h"

#include <QtWidgets/qpushbutton.h>
#include <QtGui/qicon.h>
#include <QtCore/qjsonarray.h>
#include <EngineCore/ResourceManagement/ResourceManager.h>
#include <EngineCore/Service/ServiceLocator.h>
#include <Rendering/Resources/UI.h>
#include "Editor/Utils/Utils.h"
#include <Rendering/Resources/Font.h>

using namespace Editor::Widget;

void Graphics::drawBackground(QPainter* painter, const QRectF& r)
{
    QtNodes::GraphicsView::drawBackground(painter, r);

    QPen p(QBrush(QColor(0, 0, 0)), 5.0);
    painter->setPen(p);

    QRectF canvas(QPointF(0, 0), QSizeF(canvasWidth, canvasHeight));
    painter->drawRect(canvas);
}

void Graphics::centerScene()
{
    if (scene()) 
    {
        scene()->setSceneRect(QRectF(QPointF(0, 0), QSizeF(canvasWidth, canvasHeight)));

        QRectF sceneRect = scene()->sceneRect();

        if (sceneRect.width() > this->rect().width() || sceneRect.height() > this->rect().height()) {
            fitInView(sceneRect, Qt::KeepAspectRatio);
        }

        centerOn(sceneRect.center());
    }
}

void Graphics::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    centerScene();
}

WidgetUIApp::WidgetUIApp(QSettings& pSetting, QWidget* pParent) :
	ads::CDockWidget("UI"), mModel(registerDataModels())
{
	QWidget* content = new QWidget(nullptr);
	setWidget(content);

	mLayout->setContentsMargins(5, 0, 5, 5);
	mLayout->setSpacing(0);
	content->setLayout(mLayout);

    setStyle();
    mScene = new QtNodes::DataFlowGraphicsScene(mModel, content);
    mView = new Graphics(mScene);
    
    mLayout->addWidget(mView);

    content->setAcceptDrops(true);
    setAcceptDrops(true);
    mView->viewport()->setAcceptDrops(true);
    mView->setAcceptDrops(true);

    installEventFilter(this);

    QPushButton* button = new QPushButton(QIcon(pSetting.value("MenuFileSaveMapIcon").toString()), "", content);
    connect(button, &QPushButton::pressed, this, &WidgetUIApp::saveUI);
}

void WidgetUIApp::setStyle()
{
    QtNodes::GraphicsViewStyle::setStyle(
        R"(
          {
            "GraphicsViewStyle": {
                "BackgroundColor": [53, 53, 53],
                "FineGridColor": [60, 60, 60],
                "CoarseGridColor": [25, 25, 25]
              }
          }
          )"
    );

    QtNodes::ConnectionStyle::setConnectionStyle(
        R"(
          {
            "ConnectionStyle": 
            {
              "UseDataDefinedColors": true
            }
          }
          )");
}

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> WidgetUIApp::registerDataModels()
{
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    
    ret->registerModel<UI::Button>("Button");
    ret->registerModel<UI::Image>("Image");
    ret->registerModel<UI::Text>("Text");

    return ret;
}

bool WidgetUIApp::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::GraphicsSceneDrop || event->type() == QEvent::Drop)
        return true;

    return ads::CDockWidget::eventFilter(object, event);
}

void WidgetUIApp::dragEnterEvent(QDragEnterEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void WidgetUIApp::dragMoveEvent(QDragMoveEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void WidgetUIApp::dropEvent(QDropEvent* pEvent)
{
    bool a = true;
}

void WidgetUIApp::saveUI()
{
    if (mCurrentUI.isEmpty())
        return;

    QFile file(mCurrentUI);
    if (file.open(QIODevice::WriteOnly)) 
    {
        QJsonObject sceneJson;

        QJsonArray nodesJsonArray;
        for (auto const nodeId : mModel.allNodeIds()) 
        {
            nodesJsonArray.append(saveNode(nodeId));
        }
        sceneJson["nodes"] = nodesJsonArray;

        QJsonArray connJsonArray;
        for (auto const& cid : mModel._connectivity)
        {
            connJsonArray.append(toJson(cid));
        }
        sceneJson["connections"] = connJsonArray;


        QJsonDocument doc(sceneJson);
        file.write(doc.toJson());
        file.close();

        std::string str = Editor::Utils::qStringToStdString(mCurrentUI);
        service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::UI>(str.c_str(), str)->updateSynch();
    }
}

QJsonObject WidgetUIApp::saveNode(QtNodes::NodeId const nodeId) const
{
    QJsonObject nodeJson;

    nodeJson["id"] = static_cast<qint64>(nodeId);

    nodeJson["internal-data"] = mModel._models.at(nodeId)->save();

    QPointF pos;
    {
        pos = mModel.nodeData(nodeId, QtNodes::NodeRole::Position).value<QPointF>();

        QJsonObject posJson;
        posJson["x"] = pos.x();
        posJson["y"] = pos.y();
        nodeJson["position"] = posJson;
    }

    //Ratio-pos
    {
        float width = float(pos.x()) / float(canvasWidth);
        float height = float(pos.y()) / float(canvasHeight);

        QJsonObject ratioJson;
        ratioJson["x"] = width;
        ratioJson["y"] = height;
        nodeJson["ratioPos"] = ratioJson;
    }

    QSize size;
    {
        size = mModel.nodeData(nodeId, QtNodes::NodeRole::Size).value<QSize>();

        QJsonObject sizeJson;
        sizeJson["x"] = size.width();
        sizeJson["y"] = size.height();
        nodeJson["size"] = sizeJson;
    }

    //Ratio
    {
        float width = float(size.width()) / float(canvasWidth);
        float height = float(size.height()) / float(canvasHeight);

        QJsonObject ratioJson;
        ratioJson["x"] = width;
        ratioJson["y"] = height;
        nodeJson["ratio"] = ratioJson;
    }

    return nodeJson;
}

void WidgetUIApp::open(QString pPath)
{
    toggleView(true);
    mScene->clearScene();

    QFile file(pPath);

    if (!file.open(QIODevice::ReadOnly))
        return;

    mCurrentUI = pPath;
    
    QByteArray const wholeFile = file.readAll();

    QJsonObject obj = QJsonDocument::fromJson(wholeFile).object();
    mModel.load(obj);

    QJsonArray nodesJsonArray = obj["nodes"].toArray();

    for (QJsonValueRef nodeJson : nodesJsonArray) 
    {
        QJsonObject node = nodeJson.toObject();

        auto const nodeId = node["id"].toInt();
        QJsonObject sizeJson = node["size"].toObject();
        
        QSize size(sizeJson["x"].toInt(), sizeJson["y"].toInt());

        mModel.setNodeData(nodeId, QtNodes::NodeRole::Size, size);
    }

    Q_EMIT mScene->sceneLoaded();
}

void WidgetUIApp::close()
{
    saveUI();
    toggleView(false);
}

void WidgetUIApp::updateTextSize(QtNodes::NodeDelegateModel* pData, QString text, float scale)
{
    for (auto it = mModel._models.begin(); it != mModel._models.end(); it++)
    {
        if (it->second.get() == pData)
        {
            Rendering::Resources::Font* font = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Font>("Resources/Editor/Fonts/Antonio-Bold.ttf", "Resources/Editor/Fonts/Antonio-Bold.ttf");

            std::list<Rendering::Data::Character>* listText = font->createText(Utils::qStringToStdString(text));

            float scaleX = (scale / font->mSize);
            float scaleY = scaleX * font->mSize;
            float sizeX = 0;
            for (auto it = listText->begin(); it != listText->end(); it++)
                sizeX += ((*it).glyph.advence >> 6) * scaleX;

            QSize size(sizeX, scaleY);
            mModel.setNodeData(it->first, QtNodes::NodeRole::Size, size);
            delete listText;
        }
    }
}