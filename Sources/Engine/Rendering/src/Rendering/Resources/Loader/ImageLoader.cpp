#include "Rendering/Resources/Loader/ImageLoader.h"

#ifndef STB_IMAGE_IMPLEMENTATION_
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#include <stdexcept>

using namespace Rendering::Resources::Loaders;

bool ImageLoader::exportPNG(const char* pFilePath, int w, int h, int comp, const void* data)
{
	stbi_write_png(pFilePath, w, h, comp, data, w * comp);
	return true;
}

bool ImageLoader::exportJPG(const char* pFilePath, int w, int h, int comp, const void* data)
{
	stbi_write_jpg(pFilePath, w, h, comp, data, 100);
	return true;
}

unsigned char* ImageLoader::import(const char* pFilePath, int& pWidth, int& pHeight)
{
	int texChannels;
	stbi_uc* pixels = stbi_load(pFilePath, &pWidth, &pHeight, &texChannels, STBI_rgb_alpha);
	if (!pixels)
		throw std::runtime_error("failed to load texture image!");

	return pixels;
}

void ImageLoader::cleanUpImport(unsigned char* pData)
{
	stbi_image_free(pData);
}