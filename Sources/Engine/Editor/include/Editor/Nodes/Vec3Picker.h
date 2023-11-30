#pragma once

#include <QtCore/QObject>
#include <QtGui/qevent.h>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <memory>
#include <QtWidgets/qlabel.h>
#include <Editor/Widget/WidgetDoubleSpinBox.h>

namespace Editor::Nodes
{
    class Vec3Out : public QtNodes::NodeData
    {
    public:
        QVector3D mVec3;

        Vec3Out() {};
        Vec3Out(const QVector3D& pVec3) : mVec3(pVec3) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "Vec3Out", "Vec3 out" }; }
    };


    class Vec3Picker : public QtNodes::NodeDelegateModel
    {

    public:
        QWidget* widget = nullptr;
        Editor::Widget::WidgetDoubleSpinBox* mSpinX;
        Editor::Widget::WidgetDoubleSpinBox* mSpinY;
        Editor::Widget::WidgetDoubleSpinBox* mSpinZ;
        std::shared_ptr<Vec3Out> mVec3;

        Vec3Picker();
        virtual ~Vec3Picker() {};

        void setupWidget();

        QString caption() const override { return QString("Vec3"); }

        QString name() const override { return QString("Vec3"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override;

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override {};

        QWidget* embeddedWidget() override;

        QJsonObject save() const override;

        void load(QJsonObject const& p) override;
    };
}