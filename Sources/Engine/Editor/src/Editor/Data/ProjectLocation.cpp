#include "Editor/Data/ProjectLocation.h"

using namespace Editor::Data;

ProjectLocation::ProjectLocation()
{
}

ProjectLocation::ProjectLocation(QString pFolder, QString pName) : mFolder(pFolder), mName(pName)
{
}

bool ProjectLocation::operator==(const ProjectLocation& pOther)
{
	return pOther.mFolder == mFolder && pOther.mName == mName;
}

QDataStream& Editor::Data::operator<<(QDataStream& pOut, const ProjectLocation& pValue)
{
	pOut << pValue.mFolder << pValue.mName;
	return pOut;
}

QDataStream& Editor::Data::operator>>(QDataStream& pIn, ProjectLocation& pValue)
{
	pIn >> pValue.mFolder;
	pIn >> pValue.mName;
	return pIn;
}