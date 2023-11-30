#include "Editor/UI/Button.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>
#include "Rendering/Resources/UI/IUIResource.h"
#include "Editor/Widget/WidgetEditor.h"
#include <Editor/Widget/WidgetStyleUIButton.h>

using namespace Editor::UI;

Button::Button() :
    mLabel(new Editor::Widget::WidgetDropTexture("Button"))
{
    mLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = mLabel->font();
    f.setBold(true);
    f.setItalic(true);

    mLabel->setFont(f);
    mLabel->installEventFilter(this);

    mTextStyle.hover.color = QColor(0, 0, 0, 255);
    mTextStyle.normal.color = QColor(0, 0, 0, 255);
    mTextStyle.press.color = QColor(0, 0, 0, 255);
}

unsigned int Button::nPorts(QtNodes::PortType const portType) const
{
    unsigned int result = 0;

    switch (portType) 
    {
        case QtNodes::PortType::In:
            result = 0;
            break;

        case QtNodes::PortType::Out:
            result = 0;

        default:
            break;
    }

    return result;
}

QtNodes::NodeDataType Button::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return QtNodes::NodeDataType();
}

std::shared_ptr<QtNodes::NodeData> Button::outData(QtNodes::PortIndex const port)
{
    return std::make_shared<DummyOut>();
}

QWidget* Button::embeddedWidget()
{
    return mLabel;
}

bool Button::eventFilter(QObject* object, QEvent* event)
{
    if (object == mLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            service(Editor::Widget::WidgetEditor).mApp->removeEventFilter(this);

            Editor::Widget::WidgetStyleUIButton dialog(*this);
            service(Editor::Widget::WidgetEditor).mApp->installEventFilter(&dialog);
            if (dialog.exec() == 0)
                service(Editor::Widget::WidgetEditor).mUI->saveUI();

            service(Editor::Widget::WidgetEditor).mApp->installEventFilter(this);
            return true;
        }
    }

    return QtNodes::NodeDelegateModel::eventFilter(object, event);
}




QJsonObject Button::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    modelJson["Ihover"] = saveUIStyle(&mImageStyle.hover);
    modelJson["Ipress"] = saveUIStyle(&mImageStyle.press);
    modelJson["Inormal"] = saveUIStyle(&mImageStyle.normal);

    modelJson["Thover"] = saveUIStyle(&mTextStyle.hover);
    modelJson["Tpress"] = saveUIStyle(&mTextStyle.press);
    modelJson["Tnormal"] = saveUIStyle(&mTextStyle.normal);

    modelJson["isHidden"] = isHidden;
    modelJson["zOrder"] = zOrder;

    modelJson["name"] = mName;

    QJsonObject size;
    size["x"] = mLabel->width();
    size["y"] = mLabel->height();

    modelJson["size"] = size;
    modelJson["type"] = (int)Rendering::Resources::UIResource::IUI::Button;
    modelJson["text"] = mText;
    modelJson["fontSize"] = mFontSize / Editor::Widget::canvasWidth;

    return modelJson;
}

void Button::load(QJsonObject const& pData)
{
    {
        QJsonValue h = pData["Ihover"].toObject();
        if (!h.isUndefined())
            loadStyle(h.toObject(), mImageStyle.hover);

        QJsonValue p = pData["Ipress"].toObject();
        if (!p.isUndefined())
            loadStyle(p.toObject(), mImageStyle.press);

        QJsonValue n = pData["Inormal"].toObject();
        if (!n.isUndefined())
            loadStyle(n.toObject(), mImageStyle.normal);
    }
    {
        QJsonValue h = pData["Thover"].toObject();
        if (!h.isUndefined())
            loadStyle(h.toObject(), mTextStyle.hover);

        QJsonValue p = pData["Tpress"].toObject();
        if (!p.isUndefined())
            loadStyle(p.toObject(), mTextStyle.press);

        QJsonValue n = pData["Tnormal"].toObject();
        if (!n.isUndefined())
            loadStyle(n.toObject(), mTextStyle.normal);
    }

    isHidden = pData["isHidden"].toBool();
    zOrder = pData["zOrder"].toInt();
    mName = pData["name"].toString();

    QJsonValue v = pData["size"].toObject();

    if (!v.isUndefined()) 
        mLabel->resize(v["x"].toInt(), v["y"].toInt());

    mText = pData["text"].toString();
    mFontSize = pData["fontSize"].toDouble() * Editor::Widget::canvasWidth;

    Q_EMIT dataUpdated(0);
}

QJsonObject Button::saveUIStyle(const UIStyle* pStyle) const
{
    QJsonObject objJson;

    QJsonArray nodesJsonArray;
    nodesJsonArray.append(pStyle->color.red());
    nodesJsonArray.append(pStyle->color.green());
    nodesJsonArray.append(pStyle->color.blue());
    nodesJsonArray.append(pStyle->color.alpha());

    objJson["color"] = nodesJsonArray;

    QString finalStr = pStyle->pTextPath.isEmpty() ?
        "" :
        pStyle->pTextPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];

    objJson["texture"] = Utils::qStringToStdString(finalStr).c_str();;

    return objJson;
};

void Button::loadStyle(QJsonObject& jsonStyle, UIStyle& uiStyle)
{
    if (!jsonStyle["color"].isUndefined())
    {
        QJsonArray color = jsonStyle["color"].toArray();
        uiStyle.color = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt(), color[3].toInt());
    }

    QJsonValue v = jsonStyle["texture"];

    if (!v.isUndefined())
    {
        if (v.toString().isEmpty())
            return;

        QString path = service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/" + v.toString();
        uiStyle.pTextPath = path;
    }
}