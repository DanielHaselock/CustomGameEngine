#pragma once

#include <QtCore/QObject>
#include <QtGui/qevent.h>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <memory>
#include <QtWidgets/qlabel.h>
#include <Editor/Widget/WidgetDropTexture.h>

namespace Editor::Nodes
{
    class TextureOut : public QtNodes::NodeData
    {
    public:
        QString mPath;

        TextureOut() {};
        TextureOut(const QString& pPath) : mPath(pPath) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "TextureOut", "Texture out" }; }
    };


    class TexturePicker : public QtNodes::NodeDelegateModel
    {

    public:
        Editor::Widget::WidgetDropTexture* mLabel;
        QPixmap mPixmap;
        std::shared_ptr<TextureOut> mTexture;

        TexturePicker();
        virtual ~TexturePicker() {};

        QString caption() const override { return QString("Texture"); }

        QString name() const override { return QString("Texture"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override;

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override {};

        bool eventFilter(QObject* object, QEvent* event) override;

        QWidget* embeddedWidget() override;

        QJsonObject save() const override;

        void load(QJsonObject const& p) override;
    };
}