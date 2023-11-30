#include "Rendering/Resources/Particle.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include "Tools/Utils/RandomNumberGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "rapidjson/document.h"
#include "Game/SceneSys/SceneManager.h"

#ifdef NSHIPPING
#include "Editor/Widget/WidgetEditor.h"
#include "Editor/Utils/Utils.h"
#else
#include "EngineCore/Core/EngineApp.h"
#endif

using namespace Rendering::Resources;

Particle::Particle() : mVelocity(0.0f), mLife(0.0f)
{
}

Particle::Particle(Particle& pParticle)
{
    mUniformData.mPos = pParticle.mUniformData.mPos;
    mVelocity = pParticle.mVelocity;
    mUniformData.mColor = pParticle.mUniformData.mColor;
    mLife = pParticle.mLife;
}

Particle& Particle::operator=(Particle& pParticle)
{
    mUniformData.mPos = pParticle.mUniformData.mPos;
    mVelocity = pParticle.mVelocity;
    mUniformData.mColor = pParticle.mUniformData.mColor;
    mLife = pParticle.mLife;

    return *this;
}

Particle::~Particle()
{
}

bool Particle::sort(const Particle* a, const Particle* b)
{
    return a->distCamera > b->distCamera;
}

ParticleSystem::ParticleSystem(const std::string& pFileName) : mFileName(pFileName), mUniformBuffer(VK_SHADER_STAGE_VERTEX_BIT)
{
    Tools::Utils::RandomNumberGenerator::initRNG();

    parseFile();
    mModel = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Model>("Resources/Engine/Models/FaceQuad.obj", "Resources/Engine/Models/FaceQuad.obj");
}

ParticleSystem::~ParticleSystem()
{
    for (ParticleSystem* mat : mInstance)
        delete mat;

    for (int i = 0; i < particles.size(); i++)
        delete particles[i];
    particles.clear();
}

ParticleSystem* ParticleSystem::getInstance(bool pIsEditorInstance)
{
    ParticleSystem* instance = new ParticleSystem(mFileName);
    instance->mIsEditorInstance = pIsEditorInstance;
    mInstance.push_back(instance);
    return instance;
}

void ParticleSystem::setQty(unsigned int pQty)
{
    if (mNbParticle != 0)
    {
        for (int i = 0; i < particles.size(); i++)
            delete particles[i];
        particles.clear();
    }

	mNbParticle = pQty;
	particles.reserve(pQty);
    for (int i = 0; i < pQty; i++)
        particles.push_back(new Particle());
}

void ParticleSystem::setTexture(const char* pPath)
{
    mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(pPath, pPath);
    mDefault = false;
}

int ParticleSystem::unusedParticle()
{
    for (unsigned int i = lastUsedParticle; i < particles.size(); ++i) {
        if (particles[i]->mLife <= 0.0f) 
        {
            lastUsedParticle = i;
            return i;
        }
    }
    
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (particles[i]->mLife <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    
    if (respawnFromLast)
        lastUsedParticle = particles.size() - mRespawnPart;
    else
        lastUsedParticle = 0;
    return lastUsedParticle;
}

void ParticleSystem::respawnParticle(Particle& particle, Maths::FVector3& pPos)
{
    particle.mUniformData.mPos = pPos;
    particle.mUniformData.mPos.x += Tools::Utils::RandomNumberGenerator::getDouble(-spreed.x, spreed.x);
    particle.mUniformData.mPos.y += Tools::Utils::RandomNumberGenerator::getDouble(-spreed.y, spreed.y);
    particle.mUniformData.mPos.z += Tools::Utils::RandomNumberGenerator::getDouble(-spreed.z, spreed.z);
    particle.mUniformData.mColor = mColorBegin;
    particle.mLife = mLife;
    particle.mVelocity = mVelocity * mSpeed;
}

void ParticleSystem::update(float dt, Maths::FVector3& pPos, Maths::FVector3& pCamPos, const bool& pLoop)
{
    if (finished)
        return;

    if (pLoop || spawnCount < particles.size())
        for (unsigned int i = 0; i < mRespawnPart; ++i)
        {
            int dead = unusedParticle();
            if (dead == -1)
                break;

            respawnParticle(*particles[dead], pPos);
            spawnCount++;
        }
    
    int count = 0;
    for (unsigned int i = 0; i < particles.size(); ++i)
    {
        Particle& p = *particles[i];
        p.mLife -= dt;

        if (p.mLife > 0.0f)
        {
            if (p.mVelocity == Maths::FVector3::Zero)
                p.mUniformData.mPos = pPos;
            else
                p.mUniformData.mPos -= p.mVelocity * dt;

            float t = Maths::unLerp(0, mLife, p.mLife);

            p.mUniformData.mColor.w = Maths::lerp(mColorEnd.w, mColorBegin.w, t);
            p.mUniformData.mColor.x = Maths::lerp(mColorEnd.x, mColorBegin.x, t);
            p.mUniformData.mColor.y = Maths::lerp(mColorEnd.y, mColorBegin.y, t);
            p.mUniformData.mColor.z = Maths::lerp(mColorEnd.z, mColorBegin.z, t);
            p.distCamera = Maths::FVector3::length(p.mUniformData.mPos - pCamPos);
        }
        else
        {
            count++;
            p.distCamera = -1;
        }
    }

    if (!pLoop && count == particles.size())
    {
        finished = true;
        return;
    }

    if (sortAlpha)
        std::sort(particles.begin(), particles.end(), Particle::sort);
}

void ParticleSystem::bindUniform(Rendering::Data::Material& pMat, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
    mUniformBuffer.mData.mViewProjection = pViewProj;
    mUniformBuffer.mData.mRight = Maths::FVector3(pView.data[0][0], pView.data[1][0], pView.data[2][0]);
    mUniformBuffer.mData.mUp = Maths::FVector3(pView.data[0][1], pView.data[1][1], pView.data[2][1]);
    mUniformBuffer.mData.mSize = mSize;
    mUniformBuffer.mData.mHasTexure = !mDefault;
    mUniformBuffer.updateData();
    pMat.bindDescriptor("ubo", mUniformBuffer.mDescriptorSets);
}

void ParticleSystem::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView)
{
    /*if (finished)
        return;*/

    bindUniform(pMat, pViewProj, pView);
    pMat.bindDescriptor("texSampler", mTexture->mTextureSets);

    for (unsigned int i = 0; i < particles.size(); ++i)
        if (particles[i]->mLife > 0.0f || !beingDrawAtLeastOnce)
        {
            pMat.bindConstant(pCmd, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Rendering::Data::UniformDataParticle), &particles[i]->mUniformData);
            pMat.bindPipeLine(pCmd);

            mModel->draw(pCmd);
        }

    beingDrawAtLeastOnce = true;
}

void ParticleSystem::parseFile()
{
    //Read json
    std::ifstream file(mFileName);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    rapidjson::Document doc;
    bool result = doc.Parse(buffer.str().c_str()).HasParseError();
    if (result)
        return;


    //Texture
    std::string str = doc["Result"]["Texture"].GetString();
    if (!doc["Result"]["DefaultTexture"].GetBool())
    {
#ifdef NSHIPPING
        str = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder) + "/" + str;
#else
        str = service(Game::SceneSys::SceneManager).mProjectPath + "/" + str;
#endif
        mDefault = false;
    }

    {
        static std::mutex mutex;
        std::unique_lock lock(mutex);
        mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(str.c_str(), str);
    }

    setQty(doc["Result"]["NumPart"].GetInt());

    auto coloB = doc["Result"]["colorB"].GetArray();
    mColorBegin = Maths::FVector4(coloB[0].GetInt() / 255.f, coloB[1].GetInt() / 255.f, coloB[2].GetInt() / 255.f, coloB[3].GetInt() / 255.f);

    auto coloE = doc["Result"]["colorE"].GetArray();
    mColorEnd = Maths::FVector4(coloE[0].GetInt() / 255.f, coloE[1].GetInt() / 255.f, coloE[2].GetInt() / 255.f, coloE[3].GetInt() / 255.f);

    auto velo = doc["Result"]["velocity"].GetArray();
    mVelocity = Maths::FVector3(velo[0].GetDouble(), velo[1].GetDouble(), velo[2].GetDouble());

    auto spree = doc["Result"]["spreed"].GetArray();
    spreed = Maths::FVector3(spree[0].GetDouble(), spree[1].GetDouble(), spree[2].GetDouble());

    mLife = doc["Result"]["life"].GetDouble();
    mSize = doc["Result"]["size"].GetDouble();
    mSpeed = doc["Result"]["speed"].GetDouble();
    mRespawnPart = doc["Result"]["respawnQty"].GetInt();
    sortAlpha = doc["Result"]["SortAlpha"].GetInt();
    respawnFromLast = doc["Result"]["RespawnFromLast"].GetInt();

    if (mRespawnPart > mNbParticle)
        mRespawnPart = 0;


    updateInstance();
}

void ParticleSystem::updateInstance()
{
#ifdef NSHIPPING
    Editor::Widget::WidgetEditor& editor = service(Editor::Widget::WidgetEditor);
#endif
    EngineCore::Core::EngineApp& engine = service(EngineCore::Core::EngineApp);

    for (ParticleSystem* particle : mInstance)
    {
        if (particle->mIsEditorInstance)
        {
#ifdef NSHIPPING
            editor.mScene->mainThreadAction.pushFunction([this, particle, &editor]
                { 
                    editor.mScene->renderer->waitForCleanUp();
                    copyUpdate(*particle); 
                });
#endif
        }
        else
            engine.mainThreadAction.pushFunction([this, particle, &engine]
                { 
                    engine.rend->waitForCleanUp();
                    copyUpdate(*particle); 
                });
    }
}

void ParticleSystem::copyUpdate(ParticleSystem& pParticle)
{
    pParticle.mModel = mModel;
    pParticle.mTexture = mTexture;
    pParticle.mDefault = mDefault;
    pParticle.setQty(mNbParticle);
    pParticle.mColorBegin = mColorBegin;
    pParticle.mColorEnd = mColorEnd;
    pParticle.mVelocity = mVelocity;
    pParticle.spreed = spreed;
    pParticle.mLife = mLife;
    pParticle.mSize = mSize;
    pParticle.mSpeed = mSpeed;
    pParticle.mDist = mDist;
    pParticle.mRespawnPart = mRespawnPart;
    pParticle.finished = false;
    pParticle.spawnCount = 0;
    pParticle.mFileName = mFileName;
    pParticle.lastUsedParticle = 0;
    pParticle.sortAlpha = sortAlpha;
    pParticle.respawnFromLast = respawnFromLast;
}