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
    class UnsignedFloatOut : public QtNodes::NodeData
    {
    public:
        float mFloat;

        UnsignedFloatOut() {};
        UnsignedFloatOut(const float& pFloat) : mFloat(pFloat) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedFloatOut", "Unsigned Float out" }; }
    };


    class UnsignedFloatPicker : public QtNodes::NodeDelegateModel
    {

    public:
        QWidget* widget = nullptr;
        Editor::Widget::WidgetDoubleSpinBox* mSpin;
        std::shared_ptr<UnsignedFloatOut> mFloat;

        UnsignedFloatPicker();
        virtual ~UnsignedFloatPicker() {};

        void setupWidget();

        QString caption() const override { return QString("Unsigned Float"); }

        QString name() const override { return QString("Unsigned Float"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override;

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override {};

        QWidget* embeddedWidget() override;

        QJsonObject save() const override;

        void load(QJsonObject const& p) override;
    };
}