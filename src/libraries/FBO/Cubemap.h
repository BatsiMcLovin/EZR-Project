#ifndef EZR_PROJECT_CUBEMAP_H
#define EZR_PROJECT_CUBEMAP_H

#include <Utils/debug.h>
#include "Texture.h"

namespace ezr
{
	class Cubemap : public ezr::Texture
	{
	public:
		Cubemap(std::vector<ezr::TextureData> texDatas);
		Cubemap();
	};
}
#endif //EZR_PROJECT_CUBEMAP_H