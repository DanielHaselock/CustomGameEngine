#pragma once
#include "Game/Component/CPTransform.h"
#include "Game/Utils/ComponentType.h"

using namespace Game::Component;

AComponent* CPTransform::clone()
{
	CPTransform* clone = new CPTransform(*this);
	clone->notifier.m_notificationHandlers.clear();
	clone->notifier.m_availableHandlerID = 0;
	clone->mParent = nullptr;

	return clone;
}

void CPTransform::addLocalPosition(const Maths::FVector3& pPosition)
{
	mLocalPosition += pPosition;
	updateLocalMatrix();
}

void CPTransform::addLocalRotation(const Maths::FQuaternion& pRotation)
{
	mLocalRotation = pRotation * mLocalRotation;
	updateLocalMatrix();
}

void CPTransform::addLocalScale(const Maths::FVector3& pScale)
{
	mLocalScale += pScale;
	updateLocalMatrix();
}

void CPTransform::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Transform);

	pWriter.Key("Position");
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	pWriter.StartArray();
		pWriter.Double(mLocalPosition.x);
		pWriter.Double(mLocalPosition.y);
		pWriter.Double(mLocalPosition.z);
	pWriter.EndArray();
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	pWriter.Key("Rotation");
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	pWriter.StartArray();
	pWriter.Double(mLocalRotation.x);
	pWriter.Double(mLocalRotation.y);
	pWriter.Double(mLocalRotation.z);
	pWriter.Double(mLocalRotation.w);
	pWriter.EndArray();
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	pWriter.Key("Scale");
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	pWriter.StartArray();
	pWriter.Double(mLocalScale.x);
	pWriter.Double(mLocalScale.y);
	pWriter.Double(mLocalScale.z);
	pWriter.EndArray();
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	pWriter.EndObject();
}