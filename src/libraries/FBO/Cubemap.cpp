#include "Cubemap.h"

ezr::Cubemap::Cubemap(std::vector<ezr::TextureData> texDatas)
{
	//GLuint _handle;
	glGenTextures(1, &_handle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _handle);

	/**/
	_target = GL_TEXTURE_CUBE_MAP;
	_bytesPerPixel = texDatas[0].bytesPerPixel;
	_width = texDatas[0].width;
	_height = texDatas[0].height;
	
	GLenum sides[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
						GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
						GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

	for (GLuint i = 0; i < texDatas.size(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texDatas[i].width, texDatas[i].height, 0, GL_RGB, GL_UNSIGNED_BYTE, texDatas[i].data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

ezr::Cubemap::Cubemap()
{
}
