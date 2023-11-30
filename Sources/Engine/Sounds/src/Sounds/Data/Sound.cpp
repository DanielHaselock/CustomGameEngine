#include "Sounds/Data/Sound.h"

using namespace Sounds::Data;

void Sound::setPos(const Maths::FVector3& pos)
{
	setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
}