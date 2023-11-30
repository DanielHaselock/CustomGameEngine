#include "Editor/Nodes/ParticleResult.h"
#include <Rendering/Renderer/VK/VKOffScreenRenderer.h>
#include <Rendering/Resources/VK/PipeLineBuilder.h>
#include <Rendering/Resources/Texture.h>
#include <Rendering/Buffers/VK/VKUniformBuffer.h>
#include "Rendering/Data/UniformData.h"
#include <EngineCore/ResourceManagement/ResourceManager.h>
#include <Rendering/Resources/Model.h>
#include <Editor/Widget/WidgetEditor.h>
#include "Editor/Nodes/ColorPicker.h"
#include "Editor/Nodes/TexturePicker.h"
#include "Editor/Nodes/Vec3Picker.h"
#include "Editor/Nodes/UnsignedVec3Picker.h"
#include "Editor/Nodes/UnsignedFloatPicker.h"
#include "Editor/Nodes/UnsignedIntegerPicker.h"
#include <Rendering/Resources/Loader/ImageLoader.h>
#include "QtCore/qtimer.h"

using namespace Editor::Nodes;

ParticleResult::ParticleResult() :
    mLabel(new Editor::Widget::WidgetDropTexture("Preview image"))
    , defaultText(new Rendering::Resources::Texture("Resources/Engine/Textures/ui.png"))
{
    mLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = mLabel->font();
    f.setBold(true);
    f.setItalic(true);

    mLabel->setFont(f);

    mLabel->setMinimumSize(200, 200);
    mLabel->setMaximumSize(500, 300);

    updatePreview(false);
}

unsigned int ParticleResult::nPorts(QtNodes::PortType const portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case QtNodes::PortType::In:
        result = 12;
        break;

    case QtNodes::PortType::Out:
        result = 0;

    default:
        break;
    }

    return result;
}

QtNodes::NodeDataType ParticleResult::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    if (portIndex == 0)
        return ColorBIn().type();
    else if (portIndex == 1)
        return ColorEIn().type();
    else if (portIndex == 2)
        return TextureIn().type();
    else if (portIndex == 3)
        return VeloIn().type();
    else if (portIndex == 4)
        return SpreedIn().type();
    else if (portIndex == 5)
        return LifeIn().type();
    else if (portIndex == 6)
        return SizeIn().type();
    else if (portIndex == 7)
        return SpeedIn().type(); 
    else if (portIndex == 8)
        return RespawnIn().type();
    else if (portIndex == 9)
        return NumPartIn().type();
    else if (portIndex == 10)
        return SortAlphaIn().type();
    return RespawnFromLastIn().type();
}

void ParticleResult::setInData(std::shared_ptr<QtNodes::NodeData> res, QtNodes::PortIndex const portIndex)
{
    if (portIndex == 0)
    {
        std::shared_ptr<ColorOut> obj = std::dynamic_pointer_cast<ColorOut>(res);
        if (obj)
            mColorB = std::make_shared<ColorBIn>(obj.get()->color);
        else
            mColorB.reset();
    }
    else if (portIndex == 1)
    {
        std::shared_ptr<ColorOut> obj = std::dynamic_pointer_cast<ColorOut>(res);
        if (obj)
            mColorE = std::make_shared<ColorEIn>(obj.get()->color);
        else
            mColorE.reset();
    }
    else if (portIndex == 2)
    {
        std::shared_ptr<TextureOut> obj = std::dynamic_pointer_cast<TextureOut>(res);
        if (obj)
            mTexture = std::make_shared<TextureIn>(obj.get()->mPath);
        else
            mTexture.reset();
    }
    else if (portIndex == 3)
    {
        std::shared_ptr<Vec3Out> obj = std::dynamic_pointer_cast<Vec3Out>(res);
        if (obj)
            mVelocity = std::make_shared<VeloIn>(obj.get()->mVec3);
        else
            mVelocity.reset();
    }
    else if (portIndex == 4)
    {
        std::shared_ptr<UnsignedVec3Out> obj = std::dynamic_pointer_cast<UnsignedVec3Out>(res);
        if (obj)
            mSpreed = std::make_shared<SpreedIn>(obj.get()->mVec3);
        else
            mSpreed.reset();
    }
    else if (portIndex == 5)
    {
        std::shared_ptr<UnsignedFloatOut> obj = std::dynamic_pointer_cast<UnsignedFloatOut>(res);
        if (obj)
            mLife = std::make_shared<LifeIn>(obj.get()->mFloat);
        else
            mLife.reset();
    }
    else if (portIndex == 6)
    {
        std::shared_ptr<UnsignedFloatOut> obj = std::dynamic_pointer_cast<UnsignedFloatOut>(res);
        if (obj)
            mSize = std::make_shared<SizeIn>(obj.get()->mFloat);
        else
            mSize.reset();
    }
    else if (portIndex == 7)
    {
        std::shared_ptr<UnsignedFloatOut> obj = std::dynamic_pointer_cast<UnsignedFloatOut>(res);
        if (obj)
            mSpeed = std::make_shared<SpeedIn>(obj.get()->mFloat);
        else
            mSpeed.reset();
    }
    else if (portIndex == 8)
    {
        std::shared_ptr<UnsignedIntOut> obj = std::dynamic_pointer_cast<UnsignedIntOut>(res);
        if (obj)
            mRespawn = std::make_shared<RespawnIn>(obj.get()->mInt);
        else
            mRespawn.reset();
    }
    else if (portIndex == 9)
    {
        std::shared_ptr<UnsignedIntOut> obj = std::dynamic_pointer_cast<UnsignedIntOut>(res);
        if (obj)
            mNumPart = std::make_shared<NumPartIn>(obj.get()->mInt);
        else
            mNumPart.reset();
    }
    else if (portIndex == 10)
    {
        std::shared_ptr<UnsignedIntOut> obj = std::dynamic_pointer_cast<UnsignedIntOut>(res);
        if (obj)
            mSortAlpha = std::make_shared<SortAlphaIn>(obj.get()->mInt);
        else
            mSortAlpha.reset();
    }
    else if (portIndex == 11)
    {
        std::shared_ptr<UnsignedIntOut> obj = std::dynamic_pointer_cast<UnsignedIntOut>(res);
        if (obj)
            mRespawnFromLast = std::make_shared<RespawnFromLastIn>(obj.get()->mInt);
        else
            mRespawnFromLast.reset();
    }

    //updatePreview(false);
}

void ParticleResult::updatePreview(bool pSavePreview)
{
    ////Get Model
    //EngineCore::ResourcesManagement::ResourceManager& manager = service(EngineCore::ResourcesManagement::ResourceManager);
    //Rendering::Resources::Model* model = manager.create<Rendering::Resources::Model>("Resources/Engine/Models/FaceQuad.obj", "Resources/Engine/Models/FaceQuad.obj");

    ////Create renderer and material, texture and uniform buffer
    //Rendering::Renderer::VK::VKOffScreenRenderer renderer(400, 400);
    //Rendering::Data::Material mat = Rendering::Renderer::Resources::VK::PipeLineBuilder()
    //    .initPipeLine("Resources/Engine/Shaders/ParticleVertex.vert.spv", "Resources/Engine/Shaders/ParticleFrag.frag.spv", renderer.mRenderPass, false);
    //Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataMat> unibuffer(VK_SHADER_STAGE_VERTEX_BIT);
    //

    //Rendering::Resources::Texture* textureFinal = nullptr;
    //hasText = false;
    //if (hasText = (mTexture != nullptr && mTexture.get()->mPath != nullptr))
    //    textureFinal = new Rendering::Resources::Texture(Utils::qStringToStdString(mTexture.get()->mPath));
    //else
    //    textureFinal = defaultText;

    ////Bind to material
    //mat.bindDescriptor("texSampler", textureFinal->mTextureSets);
    //mat.bindDescriptor("ubo", unibuffer.mDescriptorSets);

    ////Fill uniform buffer
    //Maths::FVector3 position(0, 0, -4);

    //bool hasColor = mColor.get() != nullptr;
    //Maths::FVector4 color = hasColor ?
    //    Maths::FVector4(mColor.get()->mColor.red(), mColor.get()->mColor.green(), mColor.get()->mColor.blue(), mColor.get()->mColor.alpha()) :
    //    Maths::FVector4(0, 0, 0, 255);

    //if (!hasColor)
    //    hasText = true;

    //unibuffer.mData;
    //unibuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
    //unibuffer.mData.mViewProjection = Maths::FMatrix4::createPerspective(45, float(renderer.mWindowExtent.width) / float(renderer.mWindowExtent.height), 0.01f, 100)
    //    * Maths::FMatrix4::lookAt(position, position + Maths::FVector3::Forward, Maths::FVector3::Up);
    //unibuffer.mData.mColor = Maths::FVector4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
    //unibuffer.mData.mAsTexture = hasText;
    //unibuffer.updateData();

    ////Draw content
    //renderer.beginFrame();
    //    mat.bindPipeLine(renderer.getCurrentCommandBuffer());
    //    model->draw(renderer.getCurrentCommandBuffer());
    //renderer.endFrame();

    ////Convert buffer to texture
    //const unsigned char* data = (const unsigned char*)renderer.pixelToArray();

    ////Pass array of pixel
    //mPixmap = QPixmap().fromImage(QImage(data, renderer.subResourceLayout.rowPitch / 4, 400, renderer.subResourceLayout.rowPitch, QImage::Format::Format_RGBX8888));
    //int w = mLabel->width();
    //int h = mLabel->height();
    //mLabel->setPixmap(mPixmap.scaled(w, h, Qt::KeepAspectRatio));
    //mLabel->adjustSize();

    //if (textureFinal != defaultText)
    //    delete textureFinal;

    //if (pSavePreview)
    //{
    //    QString& locationFolder = service(Editor::Widget::WidgetEditor).mLocation.mFolder;
    //    QSettings& settings = service(Editor::Widget::WidgetEditor).mSettings;
    //    QDir dir(locationFolder + settings.value("previewFolder").toString());
    //    if (!dir.exists())
    //        dir.mkpath(".");
    //    std::string location = Utils::qStringToStdString(dir.absolutePath());
    //    SetFileAttributes(location.c_str(), FILE_ATTRIBUTE_HIDDEN);
    //    QString filename = service(Editor::Widget::WidgetEditor).mMaterial->mCurrentMat.split(locationFolder + "/")[1];
    //    QFileInfo info(filename);
    //    filename = filename.split("." + info.suffix())[0];
    //    location += "/" + Utils::qStringToStdString(filename) + ".png";

    //    Rendering::Resources::Loaders::ImageLoader::exportPNG(location.c_str(), renderer.subResourceLayout.rowPitch / 4, 400, 4, data);
    //}
}

QWidget* ParticleResult::embeddedWidget()
{
    return mLabel;
}

QJsonObject ParticleResult::saveFinalResult()
{
    service(EngineCore::Thread::ThreadPool).queueJob([this] { updatePreview(true); });
    
    QJsonObject modelJson;
    
    //Color begin
    QJsonArray colorArrB = mColorB.get() == nullptr ?
        QJsonArray({ 255, 255, 255, 255 }) :
        QJsonArray({ mColorB.get()->mColor.red(), mColorB.get()->mColor.green(),
                    mColorB.get()->mColor.blue(), mColorB.get()->mColor.alpha() });
    modelJson["colorB"] = colorArrB;

    //Color End
    QJsonArray colorArrE = mColorE.get() == nullptr ?
        QJsonArray({ 255, 255, 255, 255 }) :
        QJsonArray({ mColorE.get()->mColor.red(), mColorE.get()->mColor.green(),
                    mColorE.get()->mColor.blue(), mColorE.get()->mColor.alpha() });
    modelJson["colorE"] = colorArrE;

    //Texture
    QString finalStr = mTexture.get() == nullptr ?
        "Resources/Engine/Textures/ui.png" :
        mTexture.get()->mPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];
    modelJson["Texture"] = Utils::qStringToStdString(finalStr).c_str();

    modelJson["HasTexture"] = hasText;
    modelJson["DefaultTexture"] = mTexture.get() == nullptr;

    //Velocity
    QJsonArray veloArr = mVelocity.get() == nullptr ?
        QJsonArray({ 0, -1, 0 }) :
        QJsonArray({ mVelocity.get()->mVec3.x(), mVelocity.get()->mVec3.y(),
                    mVelocity.get()->mVec3.z() });
    modelJson["velocity"] = veloArr;

    //Spreed
    QJsonArray spreedArr = mSpreed.get() == nullptr ?
        QJsonArray({ 3, 3, 3 }) :
        QJsonArray({ mSpreed.get()->mVec3.x(), mSpreed.get()->mVec3.y(),
                    mSpreed.get()->mVec3.z() });
    modelJson["spreed"] = spreedArr;

    //Single variable
    modelJson["life"] = mLife.get() == nullptr ? 5 : mLife.get()->mFloat;
    modelJson["size"] = mSize.get() == nullptr ? 1 : mSize.get()->mFloat;
    modelJson["speed"] = mSpeed.get() == nullptr ? 1 : mSpeed.get()->mFloat;
    modelJson["respawnQty"] = mRespawn.get() == nullptr ? 2 : mRespawn.get()->mInt;
    modelJson["NumPart"] = mNumPart.get() == nullptr ? 500 : mNumPart.get()->mInt;
    modelJson["SortAlpha"] = mSortAlpha.get() == nullptr ? 1 : mSortAlpha.get()->mInt;
    modelJson["RespawnFromLast"] = mRespawnFromLast.get() == nullptr ? 1 : mRespawnFromLast.get()->mInt;

    return modelJson;
}