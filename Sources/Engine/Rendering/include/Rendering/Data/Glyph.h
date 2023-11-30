#pragma once

#include <Maths/FVector2.h>
#include "Rendering/Resources/Texture.h"
#include <Maths/FMatrix4.h>
#include "Material.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "UniformData.h"

namespace Rendering::Data
{
	class Glyph
	{
	public:
		Glyph(Maths::FVector2 pSize, Maths::FVector2 pBearing, unsigned int advence, unsigned char* data);
		~Glyph();

		Rendering::Resources::Texture* texture = nullptr;

		Maths::FVector2 size;
		Maths::FVector2 bearing;
		unsigned int advence;
	};

	class Character
	{
	public:
		Glyph& glyph;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataText> mUniformBuffer;

		Character(Glyph& pGlyph);
		void updateProj(Maths::FMatrix4 pProj);
		void updatePos(Maths::FVector2 pos, float pScale);
		void updateColor(Maths::FVector4 color);
		void bind(Rendering::Data::Material& pMat);
	};
}