#pragma once
#include "Rendering/Resources/UI.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Game/SceneSys/SceneManager.h"
#include "Rendering/Resources/UI/Button.h"
#include "Rendering/Resources/UI/Image.h"
#include "Rendering/Resources/UI/Text.h"

#ifdef SHIPPING
    #include "EngineCore/core/EngineApp.h"
#endif

#ifdef NSHIPPING
    #include "Editor/Widget/WidgetEditor.h"
    #include "Editor/Utils/Utils.h"
    #include "Game/Component/CPUI.h"
#endif

using namespace Rendering::Resources;

UI::UI(const std::string& pFileName) : mFilename(pFileName)
{
    model = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>("Resources/Engine/Models/Cube.fbx", "Resources/Engine/Models/Cube.fbx");

    updateSynch();
}

UI::~UI()
{
    for (auto it = mResources.begin(); it != mResources.end(); it++)
    {
        for (auto ij = it->second.begin(); ij != it->second.end(); ij++)
            delete (*ij);

        it->second.clear();
    }
    mResources.clear();
}

void UI::updateProj(Maths::FMatrix4 pProj, int w, int h)
{
    for (auto it = mResources.begin(); it != mResources.end(); it++)
        for (auto ij = it->second.begin(); ij != it->second.end(); ij++)
            (*ij)->updateData(pProj, w, h);
}

void UI::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Rendering::Data::Material& pMatText)
{
    if (model == nullptr)
    {
        model = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>("Resources/Engine/Models/Cube.fbx", "Resources/Engine/Models/Cube.fbx");
        return;
    }

    for (auto it = mResources.begin(); it != mResources.end(); it++)
        for (auto ij = it->second.begin(); ij != it->second.end(); ij++)
            (*ij)->draw(pCmd, pMat, pMatText, *model);
}

bool UI::contain(Maths::FVector2 p)
{
    bool hover = false;
    for (auto it = mResources.rbegin(); it != mResources.rend(); it++)
    {
        for (auto ij = it->second.begin(); ij != it->second.end(); ij++)
        {
            if ((*ij)->isHover)
                (*ij)->onEndHover();

            if (!hover)
            {
                if ((*ij)->contain(p))
                {
                    (*ij)->onHover();
                    hover = true;
                }
            }
        }
    }

    return hover;
}

Rendering::Resources::UIResource::IUIResource* UI::containClick(Maths::FVector2 p)
{
    bool hover = false;
    Rendering::Resources::UIResource::IUIResource* pressedResource = nullptr;
    for (auto it = mResources.rbegin(); it != mResources.rend(); it++)
    {
        for (auto ij = it->second.begin(); ij != it->second.end(); ij++)
        {
            if ((*ij)->contain(p))
            {
                (*ij)->onPress();
                hover = true;
                return *ij;
            }
            else
                (*ij)->onEndHover();
        }
    }

    return pressedResource;
}

void UI::update()
{
    //Read json
    std::ifstream file(mFilename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    rapidjson::Document doc;
    bool result = doc.Parse(buffer.str().c_str()).HasParseError();
    if (result)
        return;

    const rapidjson::Value& objects = doc["nodes"].GetArray();
    for (unsigned int i = 0; i < objects.Size(); i++)
    {
        const rapidjson::Value& obj = objects[i];
        Rendering::Resources::UIResource::Rect rect;
        
        //Pos ratio
        {
            const rapidjson::Value& pos = obj["ratioPos"];
            float xPos = pos["x"].GetFloat();
            float yPos = pos["y"].GetFloat();
            rect.ratioPos = Maths::FVector2(xPos, yPos);
        }

        //Ratio
        {
            const rapidjson::Value& ratio = obj["ratio"];
            float xRatio = ratio["x"].GetFloat();
            float yRatio = ratio["y"].GetFloat();
            rect.ratio = Maths::FVector2(xRatio, yRatio);
        }
        
        Rendering::Resources::UIResource::IUI type = (Rendering::Resources::UIResource::IUI)obj["internal-data"]["type"].GetInt();
        Rendering::Resources::UIResource::IUIResource* ui = nullptr;
        switch (type)
        {
            case Rendering::Resources::UIResource::IUI::Button:
            {
                std::string text = obj["internal-data"]["text"].GetString();
                float fontSize = obj["internal-data"]["fontSize"].GetFloat();

                ui = new Rendering::Resources::UIResource::Button(rect, text, fontSize);
                break;
            }
            case Rendering::Resources::UIResource::IUI::Text:
            {
                std::string text = obj["internal-data"]["text"].GetString();
                float fontSize = obj["internal-data"]["fontSize"].GetFloat();
                ui = new Rendering::Resources::UIResource::Text(rect, text, fontSize);
                break;
            }
            case Rendering::Resources::UIResource::IUI::Image:
                ui = new Rendering::Resources::UIResource::Image(rect);
                break;
            default:
                break;
        }
        
        mResources[obj["internal-data"]["zOrder"].GetInt()].push_back(ui);
        ui->zOrder = obj["internal-data"]["zOrder"].GetInt();

        if (type != Rendering::Resources::UIResource::IUI::Button)
        {
            ui->hover = UI::loadStyle(obj["internal-data"]["hover"]);
            ui->press = UI::loadStyle(obj["internal-data"]["press"]);
            ui->normal = UI::loadStyle(obj["internal-data"]["normal"]);
        }
        else
        {
            Rendering::Resources::UIResource::Button* b = (Rendering::Resources::UIResource::Button*)ui;
            b->hover = UI::loadStyle(obj["internal-data"]["Ihover"]);
            b->press = UI::loadStyle(obj["internal-data"]["Ipress"]);
            b->normal = UI::loadStyle(obj["internal-data"]["Inormal"]);

            b->mText->hover = UI::loadStyle(obj["internal-data"]["Thover"]);
            b->mText->press = UI::loadStyle(obj["internal-data"]["Tpress"]);
            b->mText->normal = UI::loadStyle(obj["internal-data"]["Tnormal"]);
        }

        ui->isHidden = obj["internal-data"]["isHidden"].GetBool();
        ui->name = obj["internal-data"]["name"].GetString();
    }

    ready = true;

    

    EngineCore::Core::EngineApp& app = service(EngineCore::Core::EngineApp);
    updateProj(app.uiProj, app.mWidth, app.mHeight);
}

Rendering::Resources::UIResource::Style UI::loadStyle(const rapidjson::Value& obj)
{
    const rapidjson::Value& colorJson = obj["color"];

    Rendering::Resources::UIResource::Style style;
    style.color = Maths::FVector4(colorJson[0].GetInt() / 255.f,
        colorJson[1].GetInt() / 255.f,
        colorJson[2].GetInt() / 255.f,
        colorJson[3].GetInt() / 255.f);

    std::string texturePath = obj["texture"].GetString();

    style.hasTexture = !texturePath.empty();
    if (style.hasTexture)
    {
        std::string str = texturePath;
        #ifdef NSHIPPING
                str = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder) + "/" + str;
        #else
                str = service(Game::SceneSys::SceneManager).mProjectPath + "/" + str;
        #endif

        style.texture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(str.c_str(), str);
    }

    return style;
}

void UI::updateSynch(void* owner)
{
    if (mResources.empty())
        update();
    else
    {
        service(EngineCore::Core::EngineApp).mainThreadAction.pushFunction([this, owner] {
            service(EngineCore::Core::EngineApp).rend->waitForCleanUp();
            for (auto it = mResources.begin(); it != mResources.end(); it++)
            {
                for (auto ij = it->second.begin(); ij != it->second.end(); ij++)
                    delete (*ij);

                it->second.clear();
            }
            mResources.clear();
            
            update();

            #ifdef NSHIPPING
                if (service(Editor::Widget::WidgetEditor).mPlaying)
                {
                    if (owner == nullptr)
                        return;

                    Game::Data::Actor* actor = (Game::Data::Actor*)owner;
                    actor->OnAwake();
                    actor->updateWorldTransform(actor->getTransform());
                    actor->OnStart();
                }
            #endif

        });
    }
}

void UI::DelegateLuaFunction(int pHoverPress, std::function<void()> pFunction, std::string pName)
{
    for (auto& it = mResources.begin(); it != mResources.end(); it++)
    {
        for (auto& ij = it->second.begin(); ij != it->second.end(); ij++)
        {
            Rendering::Resources::UIResource::IUIResource& Ui = *(*ij);
            if (Ui.name._Equal(pName))
            {
                switch (pHoverPress)
                {
                case 0:
                    Ui.mLuaFunctionHover = pFunction;
                    break;
                case 1:
                    Ui.mLuaFunctionPress = pFunction;
                    break;
                case 2:
                    Ui.mLuaFunctionPress = pFunction;
                    Ui.mLuaFunctionHover = pFunction;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

Rendering::Resources::UIResource::IUIResource* Rendering::Resources::UI::findResource(std::string pName)
{
    for (auto it = mResources.begin(); it != mResources.end(); it++)
    {
        for (auto ij = it->second.begin(); ij != it->second.end(); ij++)
        {
            Rendering::Resources::UIResource::IUIResource* Ui = (Rendering::Resources::UIResource::IUIResource*)*ij;
            if (Ui->name._Equal(pName))
            {
                return Ui;
            }
        }
    }
    return nullptr;
}
