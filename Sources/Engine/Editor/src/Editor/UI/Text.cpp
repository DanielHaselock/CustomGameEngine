#include "Editor/UI/Text.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>
#include "Rendering/Resources/UI/IUIResource.h"
#include "Editor/Widget/WidgetEditor.h"
#include <Editor/Widget/WidgetStyleUIText.h>

using namespace Editor::UI;

Text::Text() :
    mLabel(new Editor::Widget::WidgetDropTexture("Text"))
{
    mLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = mLabel->font();
    f.setBold(true);
    f.setItalic(true);

    mLabel->setFont(f);
    mLabel->setText(mText);
    mLabel->installEventFilter(this);
}

unsigned int Text::nPorts(QtNodes::PortType const portType) const
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

QtNodes::NodeDataType Text::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return QtNodes::NodeDataType();
}

std::shared_ptr<QtNodes::NodeData> Text::outData(QtNodes::PortIndex const port)
{
    return std::make_shared<DummyOut>();
}

QWidget* Text::embeddedWidget()
{
    return mLabel;
}

QJsonObject Text::save() const
{
    QJsonObject modelJson = UIResource::save();

    QJsonObject size;
    size["x"] = mLabel->width();
    size["y"] = mLabel->height();

    modelJson["size"] = size;
    modelJson["type"] = (int)Rendering::Resources::UIResource::IUI::Text;
    modelJson["text"] = mText;
    modelJson["fontSize"] = mFontSize / Editor::Widget::canvasWidth;

    return modelJson;
}

void Text::load(QJsonObject const& p)
{
    UIResource::load(p);

    QJsonValue v = p["size"].toObject();

    if (!v.isUndefined()) 
        mLabel->resize(v["x"].toInt(), v["y"].toInt());

    mText = p["text"].toString();
    mLabel->setText(mText);
    mFontSize = p["fontSize"].toDouble() * Editor::Widget::canvasWidth;

    Q_EMIT dataUpdated(0);
}

bool Text::eventFilter(QObject* object, QEvent* event)
{
    if (object == mLabel) 
    {
        if (event->type() == QEvent::MouseButtonPress) 
        {
            service(Editor::Widget::WidgetEditor).mApp->removeEventFilter(this);

            Editor::Widget::WidgetStyleUIText dialog(*this);
            service(Editor::Widget::WidgetEditor).mApp->installEventFilter(&dialog);
            if (dialog.exec() == 0)
            {
                Editor::Widget::WidgetEditor& app = service(Editor::Widget::WidgetEditor);
                app.mUI->updateTextSize(this, mText, mFontSize);
                app.mUI->saveUI();

                mLabel->setText(mText);
                mLabel->adjustSize();
            }
            
            service(Editor::Widget::WidgetEditor).mApp->installEventFilter(this);
            return true;
        }

    }

    return QtNodes::NodeDelegateModel::eventFilter(object, event);
}