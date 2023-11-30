#include "Editor/UI/Image.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>
#include "Rendering/Resources/UI/IUIResource.h"
#include "Editor/Widget/WidgetEditor.h"
#include <Editor/Widget/WidgetStyleUIImage.h>

using namespace Editor::UI;

Image::Image() :
    mLabel(new Editor::Widget::WidgetDropTexture("Image"))
{
    mLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = mLabel->font();
    f.setBold(true);
    f.setItalic(true);

    mLabel->setFont(f);
    mLabel->installEventFilter(this);
}

unsigned int Image::nPorts(QtNodes::PortType const portType) const
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

QtNodes::NodeDataType Image::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return QtNodes::NodeDataType();
}

std::shared_ptr<QtNodes::NodeData> Image::outData(QtNodes::PortIndex const port)
{
    return std::make_shared<DummyOut>();
}

QWidget* Image::embeddedWidget()
{
    return mLabel;
}

QJsonObject Image::save() const
{
    QJsonObject modelJson = UIResource::save();

    QJsonObject size;
    size["x"] = mLabel->width();
    size["y"] = mLabel->height();

    modelJson["size"] = size;
    modelJson["type"] = (int)Rendering::Resources::UIResource::IUI::Image;

    return modelJson;
}

void Image::load(QJsonObject const& p)
{
    UIResource::load(p);

    QJsonValue v = p["size"].toObject();

    if (!v.isUndefined()) 
        mLabel->resize(v["x"].toInt(), v["y"].toInt());

    Q_EMIT dataUpdated(0);
}

bool Image::eventFilter(QObject* object, QEvent* event)
{
    if (object == mLabel) 
    {
        if (event->type() == QEvent::MouseButtonPress) 
        {
            service(Editor::Widget::WidgetEditor).mApp->removeEventFilter(this);

            Editor::Widget::WidgetStyleUIImage dialog(*this);
            service(Editor::Widget::WidgetEditor).mApp->installEventFilter(&dialog);
            if (dialog.exec() == 0)
                service(Editor::Widget::WidgetEditor).mUI->saveUI();
            
            service(Editor::Widget::WidgetEditor).mApp->installEventFilter(this);
            return true;
        }
    }

    return QtNodes::NodeDelegateModel::eventFilter(object, event);
}