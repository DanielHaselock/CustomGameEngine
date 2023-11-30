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
    class ColorBIn : public QtNodes::NodeData
    {
    public:
        QColor mColor = QColor(255, 255, 255, 255);

        ColorBIn() {};
        ColorBIn(const QColor& pColor) : mColor(pColor) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "ColorOut", "Color Begin" }; }
    };

    class ColorEIn : public QtNodes::NodeData
    {
    public:
        QColor mColor = QColor(0, 0, 0, 0);

        ColorEIn() {};
        ColorEIn(const QColor& pColor) : mColor(pColor) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "ColorOut", "Color End" }; }
    };

    class TextureIn : public QtNodes::NodeData
    {
    public:
        QString mPath;

        TextureIn() {};
        TextureIn(const QString& pPath) : mPath(pPath) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "TextureOut", "Texture in" }; }
    };

    class VeloIn : public QtNodes::NodeData
    {
    public:
        QVector3D mVec3 = QVector3D(0, 0, 0);

        VeloIn() {};
        VeloIn(const QVector3D& pVec3) : mVec3(pVec3) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "Vec3Out", "Velocity" }; }
    };

    class SpreedIn : public QtNodes::NodeData
    {
    public:
        QVector3D mVec3 = QVector3D(0, 0, 0);

        SpreedIn() {};
        SpreedIn(const QVector3D& pVec3) : mVec3(pVec3) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UVec3Out", "Spreed" }; }
    };

    class LifeIn : public QtNodes::NodeData
    {
    public:
        float mFloat = 0;

        LifeIn() {};
        LifeIn(const float& pFloat) : mFloat(pFloat) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedFloatOut", "Lifespan" }; }
    };

    class SizeIn : public QtNodes::NodeData
    {
    public:
        float mFloat = 0;

        SizeIn() {};
        SizeIn(const float& pFloat) : mFloat(pFloat) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedFloatOut", "Size" }; }
    };

    class SpeedIn : public QtNodes::NodeData
    {
    public:
        float mFloat = 0;

        SpeedIn() {};
        SpeedIn(const float& pFloat) : mFloat(pFloat) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedFloatOut", "Speed" }; }
    };

    class RespawnIn : public QtNodes::NodeData
    {
    public:
        int mInt = 0;

        RespawnIn() {};
        RespawnIn(const int& pInt) : mInt(pInt) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedIntOut", "Respawn Qty" }; }
    };

    class NumPartIn : public QtNodes::NodeData
    {
    public:
        int mInt = 0;

        NumPartIn() {};
        NumPartIn(const int& pInt) : mInt(pInt) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedIntOut", "Qty Particle" }; }
    };

    class SortAlphaIn : public QtNodes::NodeData
    {
    public:
        int mInt = 1;

        SortAlphaIn() {};
        SortAlphaIn(const int& pInt) : mInt(pInt) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedIntOut", "Sort Alpha" }; }
    };

    class RespawnFromLastIn : public QtNodes::NodeData
    {
    public:
        int mInt = 1;

        RespawnFromLastIn() {};
        RespawnFromLastIn(const int& pInt) : mInt(pInt) {};
        QtNodes::NodeDataType type() const override { return QtNodes::NodeDataType{ "UnsignedIntOut", "Respawn From Last" }; }
    };

    class ParticleResult : public QtNodes::NodeDelegateModel
    {
    public:
        QLabel* mLabel = nullptr;
        QPixmap mPixmap;
        Rendering::Resources::Texture* defaultText;
        
        std::shared_ptr<ColorBIn> mColorB;
        std::shared_ptr<ColorEIn> mColorE;
        std::shared_ptr<TextureIn> mTexture;
        std::shared_ptr<VeloIn> mVelocity;
        std::shared_ptr<SpreedIn> mSpreed;
        std::shared_ptr<LifeIn> mLife;
        std::shared_ptr<SizeIn> mSize;
        std::shared_ptr<SpeedIn> mSpeed;
        std::shared_ptr<RespawnIn> mRespawn;
        std::shared_ptr<NumPartIn> mNumPart;
        std::shared_ptr<SortAlphaIn> mSortAlpha;
        std::shared_ptr<RespawnFromLastIn> mRespawnFromLast;
        
        bool hasText = false;

        ParticleResult();

        virtual ~ParticleResult() { delete defaultText; };

        QString caption() const override { return QString("Particle Result"); }

        QString name() const override { return QString("ParticleResult"); }

        unsigned int nPorts(QtNodes::PortType const portType) const override;

        QtNodes::NodeDataType dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const override;

        std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex const port) override { return mTexture; };

        void setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex const portIndex) override;

        void updatePreview(bool pFromConstructor);

        QWidget* embeddedWidget() override;

        QJsonObject saveFinalResult();
    };
}