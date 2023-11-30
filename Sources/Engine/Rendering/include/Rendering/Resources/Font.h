#pragma once
#include "Rendering/Resources/IResource.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include <Rendering/Data/Glyph.h>

namespace Rendering::Resources
{
	class Font : public IResource
	{
		public:
			std::map<unsigned char, Rendering::Data::Glyph*> mGlyphs;
			FT_Face mFace;
			int mSize = 0;

			Font(const std::string& pPath, int pt = 48);
			~Font();

			void loadAllGlyph();
			Rendering::Data::Glyph* getGlyph(unsigned char c);

			std::list<Rendering::Data::Character>* createText(std::string text);
	};
}