#include "Editor/Nodes/UnsignedFloatPicker.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>

using namespace Editor::Nodes;

UnsignedFloatPicker::UnsignedFloatPicker()
{
    mFloat = std::make_shared<UnsignedFloatOut>();
    setupWidget();
}

void UnsignedFloatPicker::setupWidget()
{
    widget = new QWidget();
    widget->setObjectName("Component");
    QGridLayout* grid = new QGridLayout();
    widget->setLayout(grid);

    {
        QLabel* x = new QLabel("Value");
        grid->addWidget(x, 0, 0, Qt::AlignRight);

        mSpin = new Editor::Widget::WidgetDoubleSpinBox(0, 0, INT64_MAX);
        grid->addWidget(mSpin, 0, 1, Qt::AlignLeft);
        connect(mSpin, &Editor::Widget::WidgetDoubleSpinBox::valueChanged, this, [this]
        {
            mFloat.get()->mFloat = mSpin->value();

            Q_EMIT dataUpdated(0);
        });
    }
}

unsigned int UnsignedFloatPicker::nPorts(QtNodes::PortType const portType) const
{
    unsigned int result = 1;

    switch (portType) 
    {
        case QtNodes::PortType::In:
            result = 0;
            break;

        case QtNodes::PortType::Out:
            result = 1;

        default:
            break;
    }

    return result;
}

QtNodes::NodeDataType UnsignedFloatPicker::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return UnsignedFloatOut().type();
}

std::shared_ptr<QtNodes::NodeData> UnsignedFloatPicker::outData(QtNodes::PortIndex const port)
{
    return mFloat;
}

QWidget* UnsignedFloatPicker::embeddedWidget()
{
    return widget;
}

QJsonObject UnsignedFloatPicker::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    modelJson["float"] = mFloat.get() == nullptr? 0 : mFloat.get()->mFloat;

    return modelJson;
}

void UnsignedFloatPicker::load(QJsonObject const& p)
{
    QJsonValue v = p["float"];

    if (!v.isUndefined()) {
        mFloat = std::make_shared<UnsignedFloatOut>(v.toDouble());

        mSpin->setValue(mFloat.get()->mFloat);

        Q_EMIT dataUpdated(0);
    }
}