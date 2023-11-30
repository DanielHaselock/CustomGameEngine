#pragma once

#include <irrKlang/irrKlang.h>
#include <Maths/FVector3.h>

namespace Sounds::Data
{
	class Sound : public irrklang::ISound
	{
		public:
			void setPos(const Maths::FVector3& pos);
	};

}