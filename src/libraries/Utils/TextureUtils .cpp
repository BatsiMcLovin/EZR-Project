#include "TextureUtils.h"

ezr::Cubemap ezr::TextureUtils::loadCubeMapFromPaths(std::string pos_x, std::string neg_x, std::string pos_y, std::string neg_y, std::string pos_z, std::string neg_z)
{

	std::vector<ezr::TextureData> texDatas;

	texDatas.push_back(loadFromFile(pos_x));
	texDatas.push_back(loadFromFile(neg_x));
	texDatas.push_back(loadFromFile(pos_y));
	texDatas.push_back(loadFromFile(neg_y));
	texDatas.push_back(loadFromFile(pos_z));
	texDatas.push_back(loadFromFile(neg_z));
	
	return ezr::Cubemap(texDatas);
}

ezr::TextureData ezr::TextureUtils::loadFromFile(std::string path)
{	
	ezr::TextureData texData;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &texData.width, &texData.height, &texData.bytesPerPixel, 0);
	
	texData.data = data;

	ezr::debug::println("SUCCESS : " + path + " loaded. " + std::to_string(texData.width) + "x" + std::to_string(texData.height) + "px and " + std::to_string(texData.bytesPerPixel) + " bytes per channel.");
	return texData;
}
