#pragma once

namespace Rendering::Resources::Loaders
{
	class ImageLoader
	{
		public:
			static bool exportPNG(const char* pFilePath, int w, int h, int comp, const void* data);
			static bool exportJPG(const char* pFilePath, int w, int h, int comp, const void* data);
			static unsigned char* import(const char* pFilePath, int& pWidth, int& pHeight);
			static void cleanUpImport(unsigned char* pData);
	};
}