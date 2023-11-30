#include "Game/Component/CPCapsuleCollider.h"
#include <Game/Utils/ComponentType.h>
#include "Game/Data/Actor.h"

using namespace Game::Component;

CPCapsuleCollider::CPCapsuleCollider(Maths::FVector3 pPosition, float pRadius, float pHeight, void* pActor, bool shouldAddToWorld)
    : BulCapsuleCollider(pPosition, pRadius, pHeight, pActor, shouldAddToWorld)
{
}

CPCapsuleCollider::CPCapsuleCollider(const CPCapsuleCollider& pCollider)
    : BulCapsuleCollider(pCollider.mPosition, pCollider.mRadius, pCollider.mHeight, mOwner, true)
{

}

AComponent* CPCapsuleCollider::clone()
{
    return new CPCapsuleCollider(*this);
}

Game::Component::CPCapsuleCollider::~CPCapsuleCollider()
{
}

void CPCapsuleCollider::setPhysicsObject(void* pActor)
{
    mCallback->physicsObject = pActor;
}

void CPCapsuleCollider::UpdateTransform(Maths::FTransform& pTransform)
{
    setTransform(pTransform.mWorldPosition, pTransform.mWorldRotation, mRadius, mHeight);
}

void Game::Component::CPCapsuleCollider::UpdateRotation(Maths::FTransform& pTransform)
{
    setTransform(pTransform.mWorldPosition, pTransform.mWorldRotation, mRadius, mHeight);
}

void CPCapsuleCollider::recreateCollider()
{
    updateTransformWithConstraints(mLockRotX, mLockRotY, mLockRotZ);
}

Maths::FVector3 CPCapsuleCollider::getPhysicsPosition()
{
    return getPositionFromPhysics();
}




void CPCapsuleCollider::OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
    if (!isActive)
        return;

    IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

    if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
        ACollider::OnTriggerEnter(pOtherCollider, (ACollider*)pOtherCollider);
    else
        ACollider::OnCollisionEnter(pOtherCollider, (ACollider*)pOtherCollider);
}

void CPCapsuleCollider::OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
    if (!isActive)
        return;

    IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

    if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
        ACollider::OnTriggerStay(pOtherCollider, (ACollider*)pOtherCollider);
    else
        ACollider::OnCollisionStay(pOtherCollider, (ACollider*)pOtherCollider);
}

void CPCapsuleCollider::OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
    if (!isActive)
        return;

    IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

    if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
        ACollider::OnTriggerExit(pOtherCollider, (ACollider*)pOtherCollider);
    else
        ACollider::OnCollisionExit(pOtherCollider, (ACollider*)pOtherCollider);
}

Maths::FQuaternion CPCapsuleCollider::getPhysicsRotation()
{
    Maths::FQuaternion RotatedQuat = getRotationFromPhysics();
    Maths::FVector3 Angles = RotatedQuat.eulerAngles();
    Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
    Angles = Maths::FVector3(mLockRotX ? actor->getTransform()->getWorldRotation().x : Angles.x, mLockRotY ? actor->getTransform()->getWorldRotation().y :
        Angles.y, mLockRotZ ? actor->getTransform()->getWorldRotation().z : Angles.z);

    if (mLockRotX && mLockRotY && mLockRotZ)
        return actor->getTransform()->getWorldRotation();

    return Maths::FQuaternion(Angles);
}

void CPCapsuleCollider::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
    pWriter.StartObject();

    pWriter.Key("Type");
    pWriter.Int((int)Game::Utils::ComponentType::CapsuleCollider);

    pWriter.Key("Active");
    pWriter.Bool(isActive);

    pWriter.Key("TypeCollision");
    pWriter.Int((int)mType);

    pWriter.Key("Mass");
    pWriter.Double(mMass);

    pWriter.Key("Center");
    pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
    pWriter.StartArray();
    pWriter.Double(mCenter.x);
    pWriter.Double(mCenter.y);
    pWriter.Double(mCenter.z);
    pWriter.EndArray();
    pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

    pWriter.Key("Radius");
    pWriter.Double(mRadius);

    pWriter.Key("Height");
    pWriter.Double(mHeight);

    pWriter.Key("BlockRotationX");
    pWriter.Bool(mLockRotX);

    pWriter.Key("BlockRotationY");
    pWriter.Bool(mLockRotY);

    pWriter.Key("BlockRotationZ");
    pWriter.Bool(mLockRotZ);

    pWriter.EndObject();
}

void Game::Component::CPCapsuleCollider::addForce(Maths::FVector3 Force)
{
    addForcePhysics(Force);
}

void Game::Component::CPCapsuleCollider::clearForces()
{
    clearForcesPhysics();
}

void Game::Component::CPCapsuleCollider::setLinearVelocity(Maths::FVector3 pVelocity)
{
    setLinearVelocityPhysics(pVelocity);
}

void Game::Component::CPCapsuleCollider::setAngularVelocity(Maths::FVector3 pVelocity)
{
    setAngularVelocityPhysics(pVelocity);
}

Maths::FVector3 Game::Component::CPCapsuleCollider::getAngularVelocity()
{
    return getAngularVelocityPhysics();
}

Maths::FVector3 Game::Component::CPCapsuleCollider::getLinearVelocity()
{
    return getLinearVelocityPhysics();
}

void Game::Component::CPCapsuleCollider::clearLinearVelocity()
{
    clearLinearVelocityPhysics();
}

void Game::Component::CPCapsuleCollider::clearAngularVelocity()
{
    return clearAngularVelocityPhysics();
}
