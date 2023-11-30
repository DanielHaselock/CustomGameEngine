#include "Rendering/Resources/Font.h"
#include "Rendering/Resources/Loader/FontLoader.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <iostream>

using namespace Rendering::Resources;

Font::Font(const std::string& pPath, int pt) : mSize(pt)
{
	Rendering::Resources::Loaders::FontLoader::importFont(pPath.c_str(), pt, mFace);
    loadAllGlyph();

    FT_Done_Face(mFace);
    Rendering::Resources::Loaders::FontLoader::closeLib();
}

void Font::loadAllGlyph()
{
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        FT_Error error = FT_Load_Char(mFace, c, FT_LOAD_RENDER);
        if (error)
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph " << error << std::endl;
            continue;
        }

        Maths::FVector2 size(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows);
        Maths::FVector2 bearing(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top);
        unsigned int advence = mFace->glyph->bitmap.buffer != nullptr ? mFace->glyph->advance.x : mFace->glyph->advance.x * 2;
        mGlyphs[c] = new Rendering::Data::Glyph(size, bearing, advence, mFace->glyph->bitmap.buffer);
    }
}

Rendering::Data::Glyph* Font::getGlyph(unsigned char c)
{
    auto foundIt = mGlyphs.find(c);
    if (foundIt != mGlyphs.end())
        return foundIt->second;

    throw std::runtime_error("Couldn't find glyph");
}

Font::~Font()
{
    for (auto it = mGlyphs.begin(); it != mGlyphs.end(); it++)
        delete it->second;
    mGlyphs.clear();
}

std::list<Rendering::Data::Character>* Font::createText(std::string text)
{
    std::list<Rendering::Data::Character>* list = new std::list<Rendering::Data::Character>();
    for (auto it = text.begin(); it != text.end(); it++)
    {
        Rendering::Data::Glyph* character = getGlyph(*it);
        list->emplace_back(*character);
    }

    return list;
}