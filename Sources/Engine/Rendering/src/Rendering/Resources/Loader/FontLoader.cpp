#include "Rendering/Resources/Loader/FontLoader.h"
#include <stdexcept>

using namespace Rendering::Resources::Loaders;

FT_Library FontLoader::library;

bool FontLoader::importFont(const char* pFilePath, int pt, FT_Face& face)
{
    FT_Error error = FT_Init_FreeType(&library);
    if (error)
        throw std::runtime_error("couldn't init lib");

    error = FT_New_Face(library, pFilePath, 0, &face);
    if (!error) 
    {
        error = FT_Set_Pixel_Sizes(face, 0, pt);
        if (error)
            throw std::runtime_error("couldn't set pixel sizes");

        return true;
    }

    throw std::runtime_error("couldn't load font");

	return false;
}

bool FontLoader::closeLib()
{
    FT_Error error = FT_Done_FreeType(library);
    if (error)
        throw std::runtime_error("couldn't close lib");

    return true;
}