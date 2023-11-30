#include <algorithm>

#include "Tools/Utils/PathParser.h"
#include "Tools/Utils/Define.h"

using namespace Tools::Utils;

std::string PathParser::makeWindowsStyle(const std::string& pPath)
{
	std::string result;
	result.resize(pPath.size());

	for (size_t i = 0; i < pPath.size(); ++i)
		result[i] = pPath[i] == '/' ? '\\' : pPath[i];

	return result;
}

std::string PathParser::makeNonWindowsStyle(const std::string& pPath)
{
	std::string result;
	result.resize(pPath.size());

	for (size_t i = 0; i < pPath.size(); ++i)
		result[i] = pPath[i] == '\\' ? '/' : pPath[i];

	return result;
}

std::string PathParser::getContainingFolder(const std::string& pPath)
{
	std::string result;

	bool extraction = false;

	for (auto it = pPath.rbegin(); it != pPath.rend(); ++it)
	{
		if (extraction)
			result += *it;

		if (!extraction && it != pPath.rbegin() && (*it == '\\' || *it == '/'))
			extraction = true;
	}

	std::reverse(result.begin(), result.end());

	if (!result.empty() && result.back() != '\\')
		result += '\\';

	return result;
}

std::string PathParser::getElementName(const std::string& pPath)
{
	std::string result;

	std::string path = pPath;
	if (!path.empty() && path.back() == '\\')
		path.pop_back();

	//Reverse search
	for (auto it = path.rbegin(); it != path.rend() && *it != '\\' && *it != '/'; ++it)
		result += *it;

	std::reverse(result.begin(), result.end());

	return result;
}

std::string PathParser::getExtension(const std::string& pPath)
{
	std::string result;

	//Reverse search
	for (auto it = pPath.rbegin(); it != pPath.rend() && *it != '.'; ++it)
		result += *it;

	std::reverse(result.begin(), result.end());

	return result;
}

std::string PathParser::fileTypeToString(EFileType pFileType)
{
	switch (pFileType)
	{
		case PathParser::EFileType::MODEL:		return "Model";
		case PathParser::EFileType::TEXTURE:	return "Texture";
		case PathParser::EFileType::MATERIAL:	return "Material";
		case PathParser::EFileType::SOUND:		return "Sound";
		case PathParser::EFileType::SCENE:		return "Scene";
		case PathParser::EFileType::SCRIPT:		return "Script";
		case PathParser::EFileType::FONT:		return "Font";
		case PathParser::EFileType::PREFAB:		return "Prefab";
		case PathParser::EFileType::PARTICLE:		return "Particle";
	}

	return "Unknown";
}

PathParser::EFileType PathParser::getFileType(const std::string& pPath)
{
	std::string ext = getExtension(pPath);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if (ext == "fbx" || ext == "obj" || ext == "dae" || ext == "gltf")
		return EFileType::MODEL;
	else if (ext == "png" || ext == "jpeg" || ext == "jpg" || ext == "bmp")
		return EFileType::TEXTURE;
	else if (ext == Define::MATERIAL_EXTENSION)
		return EFileType::MATERIAL;
	else if (ext == "mp3")
		return EFileType::SOUND;
	else if (ext == Define::SCENE_EXTENSION)
		return EFileType::SCENE;
	else if (ext == Define::CLASS_EXTENSION)
		return EFileType::SCRIPT;
	else if (ext == "ttf")
		return EFileType::FONT;
	else if (ext == "fab")
		return EFileType::PREFAB;
	else if (ext == "pat")
		return EFileType::PARTICLE;

	return EFileType::UNKNOWN;
}
