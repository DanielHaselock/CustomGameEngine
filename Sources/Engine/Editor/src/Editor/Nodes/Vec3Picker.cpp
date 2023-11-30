#include "Editor/Nodes/Vec3Picker.h"
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qmimedata.h>
#include "Editor/Utils/Utils.h"
#include <EngineCore/Service/ServiceLocator.h>
#include <Editor/Widget/WidgetEditor.h>
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qjsonarray.h>

using namespace Editor::Nodes;

Vec3Picker::Vec3Picker()
{
    mVec3 = std::make_shared<Vec3Out>();
    setupWidget();
}

void Vec3Picker::setupWidget()
{
    widget = new QWidget();
    widget->setObjectName("Component");
    QGridLayout* grid = new QGridLayout();
    widget->setLayout(grid);

    //X
    {
        QLabel* x = new QLabel("X");
        grid->addWidget(x, 0, 0, Qt::AlignRight);

        mSpinX = new Editor::Widget::WidgetDoubleSpinBox(0, -INT64_MAX, INT64_MAX);
        grid->addWidget(mSpinX, 0, 1, Qt::AlignLeft);
        connect(mSpinX, &Editor::Widget::WidgetDoubleSpinBox::valueChanged, this, [this]
            {
                mVec3.get()->mVec3.setX(mSpinX->value());

                Q_EMIT dataUpdated(0);
            });
    }

    //Y
    {
        QLabel* y = new QLabel("Y");
        grid->addWidget(y, 1, 0, Qt::AlignRight);

        mSpinY = new Editor::Widget::WidgetDoubleSpinBox(0, -INT64_MAX, INT64_MAX);
        grid->addWidget(mSpinY, 1, 1, Qt::AlignLeft);
        connect(mSpinY, &Editor::Widget::WidgetDoubleSpinBox::valueChanged, this, [this]
            {
                mVec3.get()->mVec3.setY(mSpinY->value());

                Q_EMIT dataUpdated(0);
            });
    }

    //Z
    {
        QLabel* z = new QLabel("Z");
        grid->addWidget(z, 2, 0, Qt::AlignRight);

        mSpinZ = new Editor::Widget::WidgetDoubleSpinBox(0, -INT64_MAX, INT64_MAX);
        grid->addWidget(mSpinZ, 2, 1, Qt::AlignLeft);
        connect(mSpinZ, &Editor::Widget::WidgetDoubleSpinBox::valueChanged, this, [this]
            {
                mVec3.get()->mVec3.setZ(mSpinZ->value());

                Q_EMIT dataUpdated(0);
            });
    }

}

unsigned int Vec3Picker::nPorts(QtNodes::PortType const portType) const
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

QtNodes::NodeDataType Vec3Picker::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    return Vec3Out().type();
}

std::shared_ptr<QtNodes::NodeData> Vec3Picker::outData(QtNodes::PortIndex const port)
{
    return mVec3;
}

QWidget* Vec3Picker::embeddedWidget()
{
    
    return widget;
}

QJsonObject Vec3Picker::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    QJsonArray vec3 = mVec3.get() == nullptr ?
        QJsonArray({ 0, 0, 0}) :
        QJsonArray({ mVec3.get()->mVec3.x(), mVec3.get()->mVec3.y(),
                    mVec3.get()->mVec3.z() });

    modelJson["vec3"] = vec3;

    return modelJson;
}

void Vec3Picker::load(QJsonObject const& p)
{
    QJsonValue v = p["vec3"];

    if (!v.isUndefined()) {
        QJsonArray arr = v.toArray();

        QVector3D vec3;
        vec3.setX(arr[0].toDouble());
        vec3.setY(arr[1].toDouble());
        vec3.setZ(arr[2].toDouble());
    
        mVec3 = std::make_shared<Vec3Out>(vec3);

        mSpinX->setValue(vec3.x());
        mSpinY->setValue(vec3.y());
        mSpinZ->setValue(vec3.z());

        Q_EMIT dataUpdated(0);
    }
}