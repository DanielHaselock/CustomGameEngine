#pragma once
#include "Texture.h"
#include <Maths/FVector3.h>
#include <vector>
#include "Maths/FVector2.h"
#include "Maths/FVector4.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/UniformData.h"

namespace Rendering::Resources
{

    struct Particle
    {
        Rendering::Data::UniformDataParticle mUniformData;
        Maths::FVector3 mVelocity;
        float mLife;
        float distCamera = -1;

        static bool sort(const Particle* a, const Particle* b);

        Particle();
        Particle(Particle& pParticle);
        Particle& operator=(Particle& pParticle);
        ~Particle();
    };

    class ParticleSystem : public IResource
    {
    public:
        Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataParticleSystem> mUniformBuffer;

        Rendering::Resources::Model* mModel = nullptr;
        Rendering::Resources::Texture* mTexture = nullptr;
        bool mDefault = true;
        bool beingDrawAtLeastOnce = false;

        unsigned int lastUsedParticle = 0;
        unsigned int mNbParticle = 0;
        std::vector<Particle*> particles;

        Maths::FVector4 mColorBegin = Maths::FVector4(1, 1, 1, 1);
        Maths::FVector4 mColorEnd = Maths::FVector4(0, 0, 0, 0);
        Maths::FVector3 mVelocity = Maths::FVector3(0, -1, 0);
        Maths::FVector3 spreed = Maths::FVector3(3, 3, 3);
        float mLife = 5;
        float mSize = 1;
        float mSpeed = 1;
        unsigned int mDist = 10;
        unsigned int mRespawnPart = 2;
        bool finished = false;
        bool mIsEditorInstance = false;
        int spawnCount = 0;
        bool sortAlpha = true;
        bool  respawnFromLast = true;

        std::string mFileName;

        std::list<ParticleSystem*> mInstance;

        ParticleSystem(const std::string& pFileName);
        ~ParticleSystem();
        void setQty(unsigned int pQty);
        void setTexture(const char* pPath);

        int unusedParticle();
        void respawnParticle(Particle& particle, Maths::FVector3& pPos);
        void update(float dt, Maths::FVector3& pPos, Maths::FVector3& pCamPos, const bool& pLoop);
        void bindUniform(Rendering::Data::Material& pMat, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView);
        void draw(VkCommandBuffer pCmd, Rendering::Data::Material& pMat, Maths::FMatrix4& pViewProj, Maths::FMatrix4& pView);
        
        void parseFile();

        void updateInstance();
        void copyUpdate(ParticleSystem& pParticle);
        ParticleSystem* getInstance(bool mIsEditorInstance = false);
    };
}