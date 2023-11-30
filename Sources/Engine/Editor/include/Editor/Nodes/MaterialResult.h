#pragma once

#include <QtCore/QObject>
#include <QtGui/qevent.h>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>
#include <memory>
#include <QtWidgets/qlabel.h>
#include <Editor/Widget/WidgetDropTexture.h>
#include "Rendering/Resources/Texture.h"
#include <QtCore/qjsonarray.h>

namespace Editor::Nodes
{
    class ColorIn : public QtNodes::NodeData
    {
    public:
        QColor mColor = QColor(0, 0, 0, 1);

        ColorIn() {};
        ColorIn(const QColor& pColor) : mColor(pColor) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "ColorOut", "Albedo Color" }; }
    };

    class TextureAlbedoIn : public QtNodes::NodeData
    {
    public:
        QString mPath;

        TextureAlbedoIn() {};
        TextureAlbedoIn(const QString& pPath) : mPath(pPath) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "TextureOut", "Albedo Texture" }; }
    };

    class TextureNormalIn : public QtNodes::NodeData
    {
    public:
        QString mPath;

        TextureNormalIn() {};
        TextureNormalIn(const QString& pPath) : mPath(pPath) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "TextureOut", "Normal Texture" }; }
    };

    class TextureMetallicIn : public QtNodes::NodeData
    {
    public:
        QString mPath;

        TextureMetallicIn() {};
        TextureMetallicIn(const QString& pPath) : mPath(pPath) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "TextureOut", "Metallic Texture" }; }
    };

    class TextureRoughnessIn : public QtNodes::NodeData
    {
    public:
        QString mPath;

        TextureRoughnessIn() {};
        TextureRoughnessIn(const QString& pPath) : mPath(pPath) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "TextureOut", "Roughness Texture" }; }
    };

    class TextureAOIn : public QtNodes::NodeData
    {
    public:
        QString mPath;

        TextureAOIn() {};
        TextureAOIn(const QString& pPath) : mPath(pPath) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "TextureOut", "Ambient Occlusion Texture" }; }
    };

    class MaterialResult : public QtNodes::NodeDelegateModel
    {
    public:
        QLabel* mLabel = nullptr;
        QPixmap mPixmap;
        std::shared_ptr<ColorIn> mColor;
        std::shared_ptr<TextureAlbedoIn> mTextureAlbedo;
        std::shared_ptr<TextureNormalIn> mTextureNormal;
        std::shared_ptr<TextureMetallicIn> mTextureMetallic;
        std::shared_ptr<TextureRoughnessIn> mTextureRoughness;
        std::shared_ptr<TextureAOIn> mTextureAO;
        Rendering::Resources::Texture* defaultText;
        bool hasText = false;

        MaterialResult();

        virtual ~MaterialResult() { delete defaultText; };

        QString caption() const override { return QString("Material Result"); }

        QString name() const override { return QString("MaterialResult"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override { return mTextureAlbedo; };

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override;

        void updatePreview(bool pFromConstructor);

        QWidget* embeddedWidget() override;

        QJsonObject saveFinalResult();
    };
}