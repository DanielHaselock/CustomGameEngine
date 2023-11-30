#include "Editor/Nodes/MaterialResult.h"
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
#include <Rendering/Resources/Loader/ImageLoader.h>
#include "QtCore/qtimer.h"

using namespace Editor::Nodes;

MaterialResult::MaterialResult() :
    mLabel(new Editor::Widget::WidgetDropTexture("Preview image"))
    , defaultText(new Rendering::Resources::Texture("Resources/Engine/Textures/default.png"))
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

unsigned int MaterialResult::nPorts(QtNodes::PortType const portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case QtNodes::PortType::In:
        result = 6;
        break;

    case QtNodes::PortType::Out:
        result = 0;

    default:
        break;
    }

    return result;
}

QtNodes::NodeDataType MaterialResult::dataType(QtNodes::PortType const portType, QtNodes::PortIndex const portIndex) const
{
    if (portIndex == 0)
        return ColorIn().type();
    else if (portIndex == 1)
        return TextureAlbedoIn().type();
    else if (portIndex == 2)
        return TextureNormalIn().type();
    else if (portIndex == 3)
        return TextureMetallicIn().type();
    else if (portIndex == 4)
        return TextureRoughnessIn().type();
    return TextureAOIn().type();
}

void MaterialResult::setInData(std::shared_ptr<QtNodes::NodeData> res, QtNodes::PortIndex const portIndex)
{
    if (portIndex == 0)
    {
        std::shared_ptr<ColorOut> obj = std::dynamic_pointer_cast<ColorOut>(res);
        if (obj)
            mColor = std::make_shared<ColorIn>(obj.get()->color);
        else
            mColor.reset();
    }
    else if (portIndex == 1)
    {
        std::shared_ptr<TextureOut> obj = std::dynamic_pointer_cast<TextureOut>(res);
        if (obj)
            mTextureAlbedo = std::make_shared<TextureAlbedoIn>(obj.get()->mPath);
        else
            mTextureAlbedo.reset();
    }
    else if (portIndex == 2)
    {
        std::shared_ptr<TextureOut> obj = std::dynamic_pointer_cast<TextureOut>(res);
        if (obj)
            mTextureNormal = std::make_shared<TextureNormalIn>(obj.get()->mPath);
        else
            mTextureNormal.reset();
    }
    else if (portIndex == 3)
    {
        std::shared_ptr<TextureOut> obj = std::dynamic_pointer_cast<TextureOut>(res);
        if (obj)
            mTextureMetallic = std::make_shared<TextureMetallicIn>(obj.get()->mPath);
        else
            mTextureMetallic.reset();
    }
    else if (portIndex == 4)
    {
        std::shared_ptr<TextureOut> obj = std::dynamic_pointer_cast<TextureOut>(res);
        if (obj)
            mTextureRoughness = std::make_shared<TextureRoughnessIn>(obj.get()->mPath);
        else
            mTextureRoughness.reset();
    }
    else if (portIndex == 5)
    {
        std::shared_ptr<TextureOut> obj = std::dynamic_pointer_cast<TextureOut>(res);
        if (obj)
            mTextureAO = std::make_shared<TextureAOIn>(obj.get()->mPath);
        else
            mTextureAO.reset();
    }

    updatePreview(false);
}

void MaterialResult::updatePreview(bool pSavePreview)
{
    //Get Model
    EngineCore::ResourcesManagement::ResourceManager& manager = service(EngineCore::ResourcesManagement::ResourceManager);
    Rendering::Resources::Model* model = manager.create<Rendering::Resources::Model>("Resources/Engine/Models/Sphere.fbx", "Resources/Engine/Models/Sphere.fbx");

    //Create renderer and material, texture and uniform buffer
    Rendering::Renderer::VK::VKOffScreenRenderer renderer(400, 400);
    Rendering::Data::Material mat = Rendering::Renderer::Resources::VK::PipeLineBuilder()
        .initPipeLine("Resources/Editor/Shaders/MatVertex.vert.spv", "Resources/Editor/Shaders/MatFrag.frag.spv", renderer.mRenderPass, false, false, true, false, false, true);
    Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataMat> unibuffer(VK_SHADER_STAGE_VERTEX_BIT);
    

    Rendering::Resources::Texture* textureFinalAlbedo = nullptr;
    hasText = false;
    if (hasText = (mTextureAlbedo != nullptr && mTextureAlbedo.get()->mPath != nullptr))
    {
        std::string path = Utils::qStringToStdString(mTextureAlbedo.get()->mPath);
        textureFinalAlbedo = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(path.c_str(), path);
    }
    else
        textureFinalAlbedo = defaultText;

    Rendering::Resources::Texture* textureFinalNormal = nullptr;
    if (mTextureNormal != nullptr && mTextureNormal.get()->mPath != nullptr)
    {
        std::string path = Utils::qStringToStdString(mTextureNormal.get()->mPath);
        textureFinalNormal = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(path.c_str(), path);
    }
    else
        textureFinalNormal = defaultText;

    Rendering::Resources::Texture* textureFinalMetallic = nullptr;
    if (mTextureMetallic != nullptr && mTextureMetallic.get()->mPath != nullptr)
    {
        std::string path = Utils::qStringToStdString(mTextureMetallic.get()->mPath);
        textureFinalMetallic = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(path.c_str(), path);
    }
    else
        textureFinalMetallic = defaultText;

    Rendering::Resources::Texture* textureFinalRoughness = nullptr;
    if (mTextureRoughness != nullptr && mTextureRoughness.get()->mPath != nullptr)
    {
        std::string path = Utils::qStringToStdString(mTextureRoughness.get()->mPath);
        textureFinalRoughness = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(path.c_str(), path);
    }
    else
        textureFinalRoughness = defaultText;

    Rendering::Resources::Texture* textureFinalAO = nullptr;
    if (mTextureAO != nullptr && mTextureAO.get()->mPath != nullptr)
    {
        std::string path = Utils::qStringToStdString(mTextureAO.get()->mPath);
        textureFinalAO = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(path.c_str(), path);
    }
    else
        textureFinalAO = defaultText;

    //Bind to material
    mat.bindDescriptor("texAlbedo", textureFinalAlbedo->mTextureSets);
    mat.bindDescriptor("texNormal", textureFinalNormal->mTextureSets);
    mat.bindDescriptor("texMetallic", textureFinalMetallic->mTextureSets);
    mat.bindDescriptor("texRoughness", textureFinalRoughness->mTextureSets);
    mat.bindDescriptor("texAO", textureFinalAO->mTextureSets);
    mat.bindDescriptor("ubo", unibuffer.mDescriptorSets);

    //Fill uniform buffer
    Maths::FVector3 position(0, 0, -4);

    bool hasColor = mColor.get() != nullptr;
    Maths::FVector4 color = hasColor ?
        Maths::FVector4(mColor.get()->mColor.red(), mColor.get()->mColor.green(), mColor.get()->mColor.blue(), mColor.get()->mColor.alpha()) :
        Maths::FVector4(0, 0, 0, 255);

    if (!hasColor)
        hasText = true;

    unibuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 });
    unibuffer.mData.mViewProjection = Maths::FMatrix4::createPerspective(45, float(renderer.mWindowExtent.width) / float(renderer.mWindowExtent.height), 0.01f, 100)
        * Maths::FMatrix4::lookAt(position, position + Maths::FVector3::Forward, Maths::FVector3::Up);
    unibuffer.mData.mColor = Maths::FVector4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
    unibuffer.mData.mHasTextureAlbedo = hasText;
    unibuffer.mData.mHasTextureNormal = (int)(textureFinalNormal != defaultText);
    unibuffer.mData.mHasTextureMetalic = (int)(textureFinalMetallic != defaultText);
    unibuffer.mData.mHasTextureRoughness = (int)(textureFinalRoughness != defaultText);
    unibuffer.mData.mHasTextureAO = (int)(textureFinalAO != defaultText);
    unibuffer.updateData();

    //Draw content
    renderer.beginFrame();
        mat.bindPipeLine(renderer.getCurrentCommandBuffer());
        model->draw(renderer.getCurrentCommandBuffer());
    renderer.endFrame();

    //Convert buffer to texture
    const unsigned char* data = (const unsigned char*)renderer.pixelToArray();

    //Pass array of pixel
    mPixmap = QPixmap().fromImage(QImage(data, renderer.subResourceLayout.rowPitch / 4, 400, renderer.subResourceLayout.rowPitch, QImage::Format::Format_RGBX8888));
    int w = mLabel->width();
    int h = mLabel->height();
    mLabel->setPixmap(mPixmap.scaled(w, h, Qt::KeepAspectRatio));
    mLabel->adjustSize();

    if (pSavePreview)
    {
        QString& locationFolder = service(Editor::Widget::WidgetEditor).mLocation.mFolder;
        QSettings& settings = service(Editor::Widget::WidgetEditor).mSettings;
        QDir dir(locationFolder + settings.value("previewFolder").toString());
        if (!dir.exists())
            dir.mkpath(".");
        std::string location = Utils::qStringToStdString(dir.absolutePath());
        SetFileAttributes(location.c_str(), FILE_ATTRIBUTE_HIDDEN);
        QString filename = service(Editor::Widget::WidgetEditor).mMaterial->mCurrentMat.split(locationFolder + "/")[1];
        QFileInfo info(filename);
        filename = info.baseName();
        location += "/" + Utils::qStringToStdString(filename) + ".png";

        Rendering::Resources::Loaders::ImageLoader::exportPNG(location.c_str(), renderer.subResourceLayout.rowPitch / 4, 400, 4, data);
    }
}

QWidget* MaterialResult::embeddedWidget()
{
    return mLabel;
}

QJsonObject MaterialResult::saveFinalResult()
{
    service(EngineCore::Thread::ThreadPool).queueJob([this] { updatePreview(true); });
    
    QJsonObject modelJson;
    
    //Color
    QJsonArray colorArr = mColor.get() == nullptr ?
        QJsonArray({ 0, 0, 0, 255 }) :
        QJsonArray({ mColor.get()->mColor.red(), mColor.get()->mColor.green(),
                    mColor.get()->mColor.blue(), mColor.get()->mColor.alpha() });
    modelJson["color"] = colorArr;

    //Texture Albedo
    {
        QString finalStr = mTextureAlbedo.get() == nullptr ?
            "Resources/Engine/Textures/default.png" :
            mTextureAlbedo.get()->mPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];
        modelJson["TextureAlbedo"] = Utils::qStringToStdString(finalStr).c_str();
    }

    //Texture Normal
    {
        QString finalStr = mTextureNormal.get() == nullptr ?
            "" :
            mTextureNormal.get()->mPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];
        modelJson["TextureNormal"] = Utils::qStringToStdString(finalStr).c_str();
    }

    //Texture Metallic
    {
        QString finalStr = mTextureMetallic.get() == nullptr ?
            "" :
            mTextureMetallic.get()->mPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];
        modelJson["TextureMetallic"] = Utils::qStringToStdString(finalStr).c_str();
    }

    //Texture Roughness
    {
        QString finalStr = mTextureRoughness.get() == nullptr ?
            "" :
            mTextureRoughness.get()->mPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];
        modelJson["TextureRoughness"] = Utils::qStringToStdString(finalStr).c_str();
    }

    //Texture AO
    {
        QString finalStr = mTextureAO.get() == nullptr ?
            "" :
            mTextureAO.get()->mPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];
        modelJson["TextureAO"] = Utils::qStringToStdString(finalStr).c_str();
    }

    modelJson["HasTexture"] = hasText;
    modelJson["DefaultTexture"] = mTextureAlbedo.get() == nullptr;

    return modelJson;
}