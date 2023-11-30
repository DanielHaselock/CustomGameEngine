#include "EngineCore/Thread/ThreadPool.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Game/SceneSys/SceneManager.h"
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <sstream>
#include "Game/Component/CPModel.h"
#include "Game/Component/CPBoxCollider.h"
#include "Game/Component/CPCamera.h"
#include "Game/Component/CPCapsuleCollider.h"
#include "Game/Component/CPComplexCollider.h"
#include "Game/Component/CPScript.h"
#include "Game/Component/CPSound.h"
#include "Game/Component/CPSoundListener.h"
#include "Game/Component/CPParticle.h"
#include "Game/Component/CPUI.h"
#include "Game/Component/CPDirLight.h"
#include "Game/Component/CPPointLight.h"
#include "Game/Component/CPSpotLight.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Scripting/ScriptInterpreter.h"
#include "Game/Component/CPAnimator.h"
#include "Game/Component/CPDecal.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/Core/EngineApp.h"
#include <future>
#include <algorithm>
#include <execution>

using namespace Game::SceneSys;

SceneManager::~SceneManager()
{
	for (auto it : mScenes)
		delete it.second;
}

void SceneManager::loadScene(const char* pSceneName)
{
	if (mScenes.find(pSceneName) == mScenes.end())
		addScene(pSceneName);

	mScenes[pSceneName] = new Scene;
	mCurrentScene = mScenes[pSceneName];

    service(EngineCore::Core::EngineApp).clock.setTimeScale(1);
}

void SceneManager::loadScene(const char* pSceneName, std::string pProjectPath)
{
    if (!pProjectPath.empty())
        mProjectPath = pProjectPath;

    loadScene(pSceneName);
    parseMap(pSceneName);
}

void SceneManager::loadSceneLua(const char* pSceneName)
{
    Game::Component::CPCamera::mWorldCamera = nullptr;
    loadScene((mProjectPath + pSceneName).c_str());
    parseMap(pSceneName);
}

void SceneManager::addScene(const char* pPath)
{
	mScenes[pPath] = nullptr;
}

void SceneManager::parseMap(const char* pPath)
{
    std::ifstream file(mProjectPath + pPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    rapidjson::Document doc;
    bool result = doc.Parse(buffer.str().c_str()).HasParseError();
    if (result)
        return;
    
    service(EngineCore::Core::EngineApp).mainThreadAction.pushFunction([this] { endJobParse.flush(); });
    
    const rapidjson::Value& objects = doc["Actors"].GetArray();
    auto task = std::async(std::launch::async, [this, &objects]() {
            std::for_each(std::execution::par,
                objects.Begin(),
                objects.End(),
                [this](const rapidjson::Value& arr) {
                    deserializeObjChild(nullptr, arr);
                });
        });

    task.wait();
}

Game::Data::Actor* SceneManager::parsePrefab(const char* pPath, bool addToScene, bool relativePath)
{
    std::ifstream file;
    if (relativePath)
        file = std::ifstream(mProjectPath + pPath);
    else
        file = std::ifstream(pPath);

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    rapidjson::Document doc;
    bool result = doc.Parse(buffer.str().c_str()).HasParseError();
    if (result)
        return nullptr;

    return deserializeObjChild(nullptr, doc, addToScene);
}

Game::Data::Actor* SceneManager::deserializeObjChild(void* pParent, const rapidjson::Value& pObject, bool addToScene)
{
    Game::Data::Actor* actor = new Game::Data::Actor();

    if (addToScene)
        mCurrentScene->addActor(actor);

    if (pParent != nullptr)
    {
        (*((Game::Data::Actor*)pParent)->mChildActors.emplace_back(actor));
        actor->mParent = ((Game::Data::Actor*)pParent);
        actor->getTransform()->setParent(*((Game::Data::Actor*)pParent)->getTransform());
    }
        

    const rapidjson::Value& tags = pObject["Tags"].GetArray();
    for (unsigned int i = 0; i < tags.Size(); i++)
        actor->mTags.push_back(tags[i].GetString());

    const rapidjson::Value& objects = pObject["Components"].GetArray();
    for (unsigned int i = 0; i < objects.Size(); i++)
    {
        Game::Utils::ComponentType type = (Game::Utils::ComponentType)objects[i]["Type"].GetInt();
        if (type == Game::Utils::ComponentType::Transform)
        {
            Game::Component::CPTransform* transform = actor->getTransform();

            transform->mLocalPosition = Maths::FVector3(objects[i]["Position"][0].GetDouble(), objects[i]["Position"][1].GetDouble(), objects[i]["Position"][2].GetDouble());
            transform->mLocalRotation = Maths::FQuaternion(objects[i]["Rotation"][0].GetDouble(), objects[i]["Rotation"][1].GetDouble(), objects[i]["Rotation"][2].GetDouble(), objects[i]["Rotation"][3].GetDouble());
            transform->mLocalScale = Maths::FVector3(objects[i]["Scale"][0].GetDouble(), objects[i]["Scale"][1].GetDouble(), objects[i]["Scale"][2].GetDouble());

            actor->updateWorldTransform(transform);
        }
        else if (type == Game::Utils::ComponentType::MeshRenderer)
        {
            Game::Component::CPModel* model = new Game::Component::CPModel();
            model->isActive = objects[i]["Active"].GetBool();
            model->canReceiveDecal = objects[i]["ReceiveDecal"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                model->setModelWithPath((mProjectPath + strPath).c_str());

            model->defaultMat = objects[i]["DefaultMat"].GetBool();

            std::string strMPath = objects[i]["MatPath"].GetString();
            if (!strMPath.empty())
            {
                if (!model->defaultMat)
                    model->setMat((mProjectPath + strMPath).c_str());
                else
                    model->setMat((strMPath).c_str());
            }

            actor->addComponent(Game::Utils::ComponentType::MeshRenderer, model);
            model->setSocket(objects[i]["SocketName"].GetString());

            if (objects[i].HasMember("MeshMat"))
            {
                auto arr = objects[i]["MeshMat"].GetArray();
                for (int j = 0; j < arr.Size(); j++)
                    model->setMeshMat(arr[j]["MeshName"].GetString(), (mProjectPath + arr[j]["Mat"].GetString()).c_str());
            }
        }
        else if (type == Game::Utils::ComponentType::Animator)
        {
            Game::Component::CPAnimator* anim = new Game::Component::CPAnimator();
            anim->mCPModel = nullptr;
            anim->isActive = objects[i]["Active"].GetBool();
            anim->playOnAwake = objects[i]["PlayOnAwake"].GetBool();
            anim->loop = objects[i]["Loop"].GetBool();
            anim->cpModelIdx = objects[i]["CPModelIdx"].GetInt();
            actor->addComponent(Game::Utils::ComponentType::Animator, anim);

            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
            {
                endJobParse.pushFunction([anim, this, strPath] { anim->setAnimation((mProjectPath + strPath).c_str()); });
            }
                
        }
        else if (type == Game::Utils::ComponentType::Particle)
        {
            Game::Component::CPParticle* particle = new Game::Component::CPParticle();
            particle->isActive = objects[i]["Active"].GetBool();
            particle->playOnAwake = objects[i]["PlayOnAwake"].GetBool();
            particle->loop = objects[i]["Loop"].GetBool();
            particle->playInEditor = objects[i]["PlayInEditor"].GetBool();
            particle->mPlaybackSpeed = objects[i]["PlayBackSpeed"].GetDouble();

            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                particle->setParticle(objects[i]["Name"].GetString(), (mProjectPath + strPath).c_str());

            actor->addComponent(Game::Utils::ComponentType::Particle, particle);
        }
        else if (type == Game::Utils::ComponentType::Decal)
        {
            Game::Component::CPDecal* decal = new Game::Component::CPDecal(actor->mValueChangedFromEditor);
            decal->isActive = objects[i]["Active"].GetBool();

            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                decal->setDecal(objects[i]["Name"].GetString(), (mProjectPath + strPath).c_str());

            decal->mSize = Maths::FVector3(objects[i]["Size"][0].GetDouble(), objects[i]["Size"][1].GetDouble(), objects[i]["Size"][2].GetDouble());

            actor->addComponent(Game::Utils::ComponentType::Decal, decal);

            endJobParse.pushFunction([decal] { decal->updateDrawer(); });
        }
        else if (type == Game::Utils::ComponentType::Camera)
        {
            Game::Component::CPCamera* newCamera = new Game::Component::CPCamera(
                service(Rendering::Renderer::VK::VKRenderer).mWindow->mWidth,
                service(Rendering::Renderer::VK::VKRenderer).mWindow->mHeight,
                actor->getTransform()->mLocalPosition,
                actor->getTransform()->mLocalRotation,
                actor->mValueChangedFromEditor);
            newCamera->isActive = objects[i]["Active"].GetBool();
            newCamera->mFOV = objects[i]["FOV"].GetDouble();
            newCamera->mNear = objects[i]["Near"].GetDouble();
            newCamera->mFar = objects[i]["Far"].GetDouble();
            newCamera->useSkyBox = objects[i]["UseSkyBox"].GetBool();

            bool defaultSky = objects[i]["defaultSky"].GetBool();
            if (newCamera->useSkyBox && !defaultSky)
            {
                const rapidjson::Value& arr = objects[i]["SkyBoxPath"].GetArray();
                std::string path = mProjectPath;

                std::string left = path + arr[0].GetString();
                std::string right = path + arr[1].GetString();
                std::string top = path + arr[2].GetString();
                std::string button = path + arr[3].GetString();
                std::string front = path + arr[4].GetString();
                std::string back = path + arr[5].GetString();
                newCamera->skybox.setTexture(left, right, top, button, front, back);
            }
            else if (newCamera->useSkyBox && defaultSky)
                newCamera->skybox.setDefault();

            newCamera->updateProjection();
            actor->addComponent(Game::Utils::ComponentType::Camera, newCamera);
            newCamera->updateModel(actor->getTransform()->mWorldMatrix);
        }
        else if (type == Game::Utils::ComponentType::BoxCollider)
        {
            Maths::FVector3 position = actor->getTransform()->mWorldPosition;
            Maths::FVector3 size = Maths::FVector3(objects[i]["Size"][0].GetDouble(), objects[i]["Size"][1].GetDouble(), objects[i]["Size"][2].GetDouble()) * actor->getTransform()->mWorldScale;

            Game::Component::CPBoxCollider* newBody = new Game::Component::CPBoxCollider(position, size,(void*)actor, addToScene);
            newBody->isActive = objects[i]["Active"].GetBool();
            newBody->mType = (Physics::Data::TypeRigidBody)objects[i]["TypeCollision"].GetInt();
            //newBody->setType(newBody->mColliderType);
            //newBody->setMass(objects[i]["Mass"].GetDouble());
            newBody->mMass = objects[i]["Mass"].GetDouble();
            newBody->mCenter = Maths::FVector3(objects[i]["Center"][0].GetDouble(), objects[i]["Center"][1].GetDouble(), objects[i]["Center"][2].GetDouble());
            newBody->mBaseSize = Maths::FVector3(objects[i]["Size"][0].GetDouble(), objects[i]["Size"][1].GetDouble(), objects[i]["Size"][2].GetDouble());
            newBody->mLockRotX = objects[i]["BlockRotationX"].GetBool();
            newBody->mLockRotY = objects[i]["BlockRotationY"].GetBool();
            newBody->mLockRotZ = objects[i]["BlockRotationZ"].GetBool();
            //newBody->recreateCollider();

            actor->addComponent(Game::Utils::ComponentType::Collider, newBody);

            newBody->UpdateTransform(*actor->getTransform());

            if (newBody->mType == Physics::Data::TypeRigidBody::E_DYNAMIC)
                actor->detachFromParent();
        }
        else if (type == Game::Utils::ComponentType::CapsuleCollider)
        {
            Maths::FVector3 position = actor->getTransform()->mWorldPosition;
            float Radius = Maths::fmax(actor->getTransform()->mWorldScale.x, actor->getTransform()->mWorldScale.z);
            float Height = actor->getTransform()->mWorldScale.y;

            Game::Component::CPCapsuleCollider* newBody = new Game::Component::CPCapsuleCollider(position, Radius, Height, (void*)actor, addToScene);
            newBody->isActive = objects[i]["Active"].GetBool();
            newBody->mType = (Physics::Data::TypeRigidBody)objects[i]["TypeCollision"].GetInt();
            //newBody->setType(newBody->mColliderType);
            //newBody->setMass(objects[i]["Mass"].GetDouble());
            newBody->mCenter = Maths::FVector3(objects[i]["Center"][0].GetDouble(), objects[i]["Center"][1].GetDouble(), objects[i]["Center"][2].GetDouble());
            newBody->mMass = objects[i]["Mass"].GetDouble();
            newBody->mRadius = objects[i]["Radius"].GetDouble();
            newBody->mHeight = objects[i]["Height"].GetDouble();
            newBody->mLockRotX = objects[i]["BlockRotationX"].GetBool();
            newBody->mLockRotY = objects[i]["BlockRotationY"].GetBool();
            newBody->mLockRotZ = objects[i]["BlockRotationZ"].GetBool();
            //newBody->recreateCollider();

            actor->addComponent(Game::Utils::ComponentType::Collider, newBody);

            newBody->UpdateTransform(*actor->getTransform());

            if (newBody->mType == Physics::Data::TypeRigidBody::E_DYNAMIC)
                actor->detachFromParent();
        }
        else if (type == Game::Utils::ComponentType::ComplexCollider)
        {
            Maths::FVector3 position = actor->getTransform()->mWorldPosition;
            Maths::FQuaternion rotation = actor->getTransform()->mWorldRotation;
            Maths::FVector3 size = actor->getTransform()->mWorldScale;

            Game::Component::CPComplexCollider* newBody = new Game::Component::CPComplexCollider(position, rotation, size, actor->mComponents[Game::Utils::ComponentType::MeshRenderer], (void*)actor, addToScene);
            newBody->isActive = objects[i]["Active"].GetBool();
            newBody->mType = (Physics::Data::TypeRigidBody)objects[i]["TypeCollision"].GetInt();
            //newBody->setType(newBody->mColliderType);
            //newBody->setMass(objects[i]["Mass"].GetDouble());
            newBody->mMass = objects[i]["Mass"].GetDouble();

            newBody->recreateCollider();

            actor->addComponent(Game::Utils::ComponentType::Collider, newBody);
        }
        else if (type == Game::Utils::ComponentType::Script)
        {
            Game::Component::CPScript* script = new Game::Component::CPScript();
            script->isActive = objects[i]["Active"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                script->setScript(objects[i]["Name"].GetString(), (mProjectPath + strPath).c_str());
            actor->addComponent(Game::Utils::ComponentType::Script, script);
            service(Scripting::ScriptInterpreter).mBehaviours.push_back(script);
            service(Scripting::ScriptInterpreter).registerScript(script);
        }
        else if (type == Game::Utils::ComponentType::Sound)
        {
            Game::Component::CPSound* sound = new Game::Component::CPSound();
            sound->isActive = objects[i]["Active"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                sound->setClip(objects[i]["Name"].GetString(), (mProjectPath + strPath).c_str());
            sound->mute = objects[i]["Mute"].GetBool();
            sound->playOnAwake = objects[i]["PlayOnAwake"].GetBool();
            sound->loop = objects[i]["Loop"].GetBool();
            sound->volume = objects[i]["Volume"].GetInt();
            sound->is3DSound = objects[i]["Is3DSound"].GetBool();
            sound->minDist = objects[i]["MinDist"].GetInt();
            actor->addComponent(Game::Utils::ComponentType::Sound, sound);
        }
        else if (type == Game::Utils::ComponentType::SoundListener)
        {
            Game::Component::CPSoundListener* sound = new Game::Component::CPSoundListener();
            sound->isActive = objects[i]["Active"].GetBool();
            actor->addComponent(Game::Utils::ComponentType::SoundListener, sound);
        }
        else if (type == Game::Utils::ComponentType::UI)
        {
            Game::Component::CPUI* ui = new Game::Component::CPUI();
            ui->isActive = objects[i]["Active"].GetBool();
            std::string strPath = objects[i]["Path"].GetString();
            if (!strPath.empty())
                ui->setUI(objects[i]["Name"].GetString(), (mProjectPath + strPath).c_str());
            actor->addComponent(Game::Utils::ComponentType::UI, ui);
        }
        else if (type == Game::Utils::ComponentType::DirLight)
        {
            Game::Component::CPDirLight* dirLight = new Game::Component::CPDirLight();
            dirLight->isActive = objects[i]["Active"].GetBool();
            dirLight->mAttenuatuion = objects[i]["Attenuation"].GetDouble();
            dirLight->mColor = Maths::FVector3(objects[i]["Color"][0].GetDouble(), objects[i]["Color"][1].GetDouble(), objects[i]["Color"][2].GetDouble());
            dirLight->mCastShadow = objects[i]["CastShadow"].GetBool();
            dirLight->mBrightness = objects[i]["Brightness"].GetDouble();

            actor->addComponent(Game::Utils::ComponentType::Light, dirLight);
        }
        else if (type == Game::Utils::ComponentType::PointLight)
        {
            Game::Component::CPPointLight* pointLight = new Game::Component::CPPointLight();
            pointLight->isActive = objects[i]["Active"].GetBool();
            pointLight->mAttenuatuion = objects[i]["Attenuation"].GetDouble();
            pointLight->mColor = Maths::FVector3(objects[i]["Color"][0].GetDouble(), objects[i]["Color"][1].GetDouble(), objects[i]["Color"][2].GetDouble());
            pointLight->mRadius = objects[i]["Radius"].GetDouble();
            pointLight->mBrightness = objects[i]["Brightness"].GetDouble();
            pointLight->mCastShadow = objects[i]["CastShadow"].GetBool();

            actor->addComponent(Game::Utils::ComponentType::Light, pointLight);
        }
        else if (type == Game::Utils::ComponentType::SpotLight)
        {
            Game::Component::CPSpotLight* spotLight = new Game::Component::CPSpotLight();
            spotLight->isActive = objects[i]["Active"].GetBool();
            spotLight->mAttenuatuion = objects[i]["Attenuation"].GetDouble();
            spotLight->mColor = Maths::FVector3(objects[i]["Color"][0].GetDouble(), objects[i]["Color"][1].GetDouble(), objects[i]["Color"][2].GetDouble());
            spotLight->mCutOff = objects[i]["CutOff"].GetDouble();
            spotLight->mOuterCutOff = objects[i]["OuterCutOff"].GetDouble();
            spotLight->mBrightness = objects[i]["Brightness"].GetDouble();
            spotLight->mCastShadow = objects[i]["CastShadow"].GetBool();
            spotLight->updateRad();

            actor->addComponent(Game::Utils::ComponentType::Light, spotLight);
        }
    }

    const rapidjson::Value& childs = pObject["Childs"].GetArray();
    for (unsigned int i = 0; i < childs.Size(); i++)
        deserializeObjChild(actor, childs[i], addToScene);

    return actor;
}
