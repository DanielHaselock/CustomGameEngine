#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <memory>
#include <QtWidgets/qcolordialog.h>

namespace Editor::Nodes
{
    class ColorOut : public QtNodes::NodeData
    {
        public:
            QColor color;

            ColorOut() {};
            ColorOut(const QColor& pColor) : color(pColor) {};
            QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "ColorOut", "Color out" }; }
    };


    class ColorPicker : public QtNodes::NodeDelegateModel
    {

    public:
        QColorDialog* dialog = nullptr;
        QWidget* widget = nullptr;
        std::shared_ptr<ColorOut> mColor;

        ColorPicker();
        virtual ~ColorPicker() {};

        QString caption() const override { return QString("Color"); }

        QString name() const override { return QString("Color4"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override;

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override {};

        QWidget* embeddedWidget() override;

        void onColorChange(const QColor& pColor);

        QJsonObject save() const override;

        void load(QJsonObject const& p) override;
    };
}