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
    class UnsignedIntOut : public QtNodes::NodeData
    {
    public:
        int mInt = 0;

        UnsignedIntOut() {};
        UnsignedIntOut(const int& pInt) : mInt(pInt) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedIntOut", "Unsigned Int out" }; }
    };


    class UnsignedIntegerPicker : public QtNodes::NodeDelegateModel
    {

    public:
        QWidget* widget = nullptr;
        Editor::Widget::WidgetDoubleSpinBox* mSpin;
        std::shared_ptr<UnsignedIntOut> mInt;

        UnsignedIntegerPicker();
        virtual ~UnsignedIntegerPicker() {};

        void setupWidget();

        QString caption() const override { return QString("Usigned Integer"); }

        QString name() const override { return QString("Usigned Integer"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override;

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override {};

        QWidget* embeddedWidget() override;

        QJsonObject save() const override;

        void load(QJsonObject const& p) override;
    };
}