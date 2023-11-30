#include "Editor/Nodes/IntegerPicker.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>

using namespace Editor::Nodes;

IntegerPicker::IntegerPicker()
{
    mInt = std::make_shared<IntOut>();
    setupWidget();
}

void IntegerPicker::setupWidget()
{
    widget = new QWidget();
    widget->setObjectName("Component");
    QGridLayout* grid = new QGridLayout();
    widget->setLayout(grid);

    {
        QLabel* x = new QLabel("Value");
        grid->addWidget(x, 0, 0, Qt::AlignRight);

        mSpin = new Editor::Widget::WidgetDoubleSpinBox(0, -INT64_MAX, INT64_MAX, 1);
        grid->addWidget(mSpin, 0, 1, Qt::AlignLeft);
        connect(mSpin, &Editor::Widget::WidgetDoubleSpinBox::valueChanged, this, [this]
        {
            mInt.get()->mInt = mSpin->value();

            Q_EMIT dataUpdated(0);
        });
    }
}

unsigned int IntegerPicker::nPorts(QtNodes::PortType const portType) const
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

QtNodes::NodeDataType IntegerPicker::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return IntOut().type();
}

std::shared_ptr<QtNodes::NodeData> IntegerPicker::outData(QtNodes::PortIndex const port)
{
    return mInt;
}

QWidget* IntegerPicker::embeddedWidget()
{
    return widget;
}

QJsonObject IntegerPicker::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    modelJson["int"] = mInt.get() == nullptr? 0 : mInt.get()->mInt;

    return modelJson;
}

void IntegerPicker::load(QJsonObject const& p)
{
    QJsonValue v = p["int"];

    if (!v.isUndefined()) {
        mInt = std::make_shared<IntOut>(v.toInt());

        mSpin->setValue(mInt.get()->mInt);

        Q_EMIT dataUpdated(0);
    }
}