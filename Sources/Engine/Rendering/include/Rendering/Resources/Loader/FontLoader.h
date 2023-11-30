#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Rendering::Resources::Loaders
{
	class FontLoader
	{
	public:
		static FT_Library library;

		static bool importFont(const char* pFilePath, int pt, FT_Face& face);
		static bool closeLib();

	};
}