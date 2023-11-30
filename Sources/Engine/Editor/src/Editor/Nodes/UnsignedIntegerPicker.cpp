#include "Editor/Nodes/UnsignedIntegerPicker.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>

using namespace Editor::Nodes;

UnsignedIntegerPicker::UnsignedIntegerPicker()
{
    mInt = std::make_shared<UnsignedIntOut>();
    setupWidget();
}

void UnsignedIntegerPicker::setupWidget()
{
    widget = new QWidget();
    widget->setObjectName("Component");
    QGridLayout* grid = new QGridLayout();
    widget->setLayout(grid);

    {
        QLabel* x = new QLabel("Value");
        grid->addWidget(x, 0, 0, Qt::AlignRight);

        mSpin = new Editor::Widget::WidgetDoubleSpinBox(0, 0, INT64_MAX, 1);
        grid->addWidget(mSpin, 0, 1, Qt::AlignLeft);
        connect(mSpin, &Editor::Widget::WidgetDoubleSpinBox::valueChanged, this, [this]
        {
            mInt.get()->mInt = mSpin->value();

            Q_EMIT dataUpdated(0);
        });
    }
}

unsigned int UnsignedIntegerPicker::nPorts(QtNodes::PortType const portType) const
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

QtNodes::NodeDataType UnsignedIntegerPicker::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return UnsignedIntOut().type();
}

std::shared_ptr<QtNodes::NodeData> UnsignedIntegerPicker::outData(QtNodes::PortIndex const port)
{
    return mInt;
}

QWidget* UnsignedIntegerPicker::embeddedWidget()
{
    return widget;
}

QJsonObject UnsignedIntegerPicker::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    modelJson["int"] = mInt.get() == nullptr? 0 : mInt.get()->mInt;

    return modelJson;
}

void UnsignedIntegerPicker::load(QJsonObject const& p)
{
    QJsonValue v = p["int"];

    if (!v.isUndefined()) {
        mInt = std::make_shared<UnsignedIntOut>(v.toInt());

        mSpin->setValue(mInt.get()->mInt);

        Q_EMIT dataUpdated(0);
    }
}