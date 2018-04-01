#ifndef EZR_PROJECT_TEXTUREUTILS_H
#define EZR_PROJECT_TEXTUREUTILS_H

#include "FBO/Cubemap.h"
#include "FBO/Texture.h"

namespace ezr
{

	class TextureUtils
	{
	public:
		static ezr::Cubemap loadCubeMapFromPaths(	std::string pos_x,
											std::string neg_x,
											std::string pos_y,
											std::string neg_y,
											std::string pos_z,
											std::string neg_z);

		static ezr::TextureData loadFromFile(std::string path);
	};
}
#endif