#include "Game/Component/CPBoxCollider.h"
#include "Physics/Data/TypeRigidBody.h"
#include "Game/Component/CPCapsuleCollider.h"
#include <Game/Utils/ComponentType.h>
using namespace Game::Component;

CPBoxCollider::CPBoxCollider(Maths::FVector3 pPosition, Maths::FVector3 pSize, void* pActor, bool shouldAddToWorld)
    : BulBoxCollider(pPosition, pSize, pActor, shouldAddToWorld)
{
}

CPBoxCollider::CPBoxCollider(const CPBoxCollider& pCollider)
    : BulBoxCollider(pCollider.mPosition, pCollider.mSize, mOwner, true)
{
    
}

AComponent* CPBoxCollider::clone()
{
    return new CPBoxCollider(*this);
}

void CPBoxCollider::setPhysicsObject(void* pActor)
{
    mCallback->physicsObject = pActor;
}

void CPBoxCollider::UpdateTransform(Maths::FTransform& pTransform)
{
    setTransform(pTransform.mWorldPosition, pTransform.mWorldRotation, pTransform.mWorldScale);
}

void Game::Component::CPBoxCollider::UpdateRotation(Maths::FTransform& pTransform)
{
    setTransform(pTransform.mWorldPosition, pTransform.mWorldRotation, pTransform.mWorldScale);
}

void CPBoxCollider::recreateCollider()
{
    updateTransformWithConstraints(mLockRotX, mLockRotY, mLockRotZ);
}

Maths::FVector3 CPBoxCollider::getPhysicsPosition()
{
    return getPositionFromPhysics();
}

Maths::FQuaternion CPBoxCollider::getPhysicsRotation()
{
    Maths::FQuaternion RotatedQuat = getRotationFromPhysics();
    Maths::FVector3 Angles = RotatedQuat.eulerAngles();

    return Maths::FQuaternion(Angles);
}



Game::Component::CPBoxCollider::~CPBoxCollider()
{
}

void CPBoxCollider::OnAnyCollisionEnter(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
    if (!isActive)
        return;

    IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

    if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
        ACollider::OnTriggerEnter(pOtherCollider, (ACollider*)pRigidbody);
    else
        ACollider::OnCollisionEnter(pOtherCollider, (ACollider*)pOtherCollider);
}

void CPBoxCollider::OnAnyCollisionStay(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
    if (!isActive)
        return;

    IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

    if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
        ACollider::OnTriggerStay(pOtherCollider, (ACollider*)pOtherCollider);
    else
        ACollider::OnCollisionStay(pOtherCollider, (ACollider*)pOtherCollider);
}

void CPBoxCollider::OnAnyCollisionExit(void* pRigidbody, void* pOtherCollider, btPersistentManifold* persistent)
{
    if (!isActive)
        return;

    IRigidbody* OtherCollider = (IRigidbody*)pRigidbody;

    if (mType == Physics::Data::TypeRigidBody::E_TRIGGER || OtherCollider->mType == Physics::Data::TypeRigidBody::E_TRIGGER)
        ACollider::OnTriggerExit(pOtherCollider, (ACollider*)pOtherCollider);
    else
        ACollider::OnCollisionExit(pOtherCollider, (ACollider*)pOtherCollider);
}

void CPBoxCollider::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
    pWriter.StartObject();

    pWriter.Key("Type");
    pWriter.Int((int)Game::Utils::ComponentType::BoxCollider);

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

    pWriter.Key("Size");
    pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
    pWriter.StartArray();
    pWriter.Double(mBaseSize.x);
    pWriter.Double(mBaseSize.y);
    pWriter.Double(mBaseSize.z);
    pWriter.EndArray();
    pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

    pWriter.Key("BlockRotationX");
    pWriter.Bool(mLockRotX);

    pWriter.Key("BlockRotationY");
    pWriter.Bool(mLockRotY);

    pWriter.Key("BlockRotationZ");
    pWriter.Bool(mLockRotZ);

    pWriter.EndObject();
}

void Game::Component::CPBoxCollider::addForce(Maths::FVector3 Force)
{
    addForcePhysics(Force);
}

void Game::Component::CPBoxCollider::clearForces()
{
    clearForcesPhysics();
}

void Game::Component::CPBoxCollider::setLinearVelocity(Maths::FVector3 pVelocity)
{
    setLinearVelocityPhysics(pVelocity);
}

void Game::Component::CPBoxCollider::setAngularVelocity(Maths::FVector3 pVelocity)
{
    setAngularVelocityPhysics(pVelocity);
}

Maths::FVector3 Game::Component::CPBoxCollider::getAngularVelocity()
{
    return getAngularVelocityPhysics();
}

Maths::FVector3 Game::Component::CPBoxCollider::getLinearVelocity()
{
    return getLinearVelocityPhysics();
}

void Game::Component::CPBoxCollider::clearLinearVelocity()
{
    clearLinearVelocityPhysics();
}

void Game::Component::CPBoxCollider::clearAngularVelocity()
{
    return clearAngularVelocityPhysics();
}
