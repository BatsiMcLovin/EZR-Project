//
// Created by dino on 04.12.15.
//

#include "Texture.h"

#include <iterator>
#include <algorithm>


ezr::Texture::Texture()
{

}

ezr::Texture::Texture(ezr::TextureData texData)
{
	_target = GL_TEXTURE_2D;
	generateTexture(texData);
}

ezr::Texture::Texture(std::string fileName)
{
    _target = GL_TEXTURE_2D;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fileName.c_str(), &_width, &_height, &_bytesPerPixel, 0);

	ezr::TextureData texData;
	texData.data = data;
	texData.bytesPerPixel = _bytesPerPixel;
	texData.width = _width;
	texData.height = _height;

	generateTexture(texData);
   
    stbi_image_free(data);

    std::cout << "SUCCESS: image loaded from " << fileName << std::endl;
}

ezr::Texture::Texture(GLuint handle, GLuint width, GLuint height)
{
    _target = GL_TEXTURE_2D;
    _handle = handle;
    _width = width;
    _height = height;
}

void ezr::Texture::bind()
{
    glBindTexture(_target, _handle);
    glGenerateMipmap(_target);
}

void ezr::Texture::unbind()
{
    glBindTexture(_target, 0);
    glGenerateMipmap(_target);
}

void ezr::Texture::bindImageTexture(GLuint unit, GLenum access, GLenum format)
{
	glBindImageTexture(unit, getTexHandle(), 0, GL_FALSE, 0, access, format);
}

GLuint ezr::Texture::getTexHandle()
{
    return _handle;
}

void ezr::Texture::setTarget(GLenum target)
{
    _target = target;
}

glm::ivec2 ezr::Texture::getSize()
{
    return glm::ivec2(_width, _height);
}

void ezr::Texture::generateTexture(ezr::TextureData texData)
{
	_width = texData.width;
	_height = texData.height;
	_bytesPerPixel = texData.bytesPerPixel;

	//create new texture
	glGenTextures(1, &_handle);

	//bind the texture
	glBindTexture(GL_TEXTURE_2D, _handle);

	//send image data to the new texture
	if (_bytesPerPixel < 3) {
		std::cout << "ERROR: Unable to open image" << std::endl;
		return;
	}
	else if (_bytesPerPixel == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData.data);
		ezr::debug::println("Loading 3 channel RGBA...");
	}
	else if (_bytesPerPixel == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data);
		ezr::debug::println("Loading 4 channel RGBA...");
	}
	else {
		ezr::debug::println("Unknown format for bytes per pixel... Changed to \"4\"");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data);
	}

	//texture settings
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ezr::Texture::free()
{
    glDeleteTextures(1, &_handle);
    _handle = 0;
}

ezr::Texture::Texture(ezr::Texture* texture)
{
    if(texture == NULL) { return; }

    _handle = texture->_handle;
    _width = texture->getSize().x;
    _height = texture->getSize().y;
    _bytesPerPixel = texture->_bytesPerPixel;
}

ezr::Texture ezr::Texture::getMipMapTexture(int level)
{
//    if(level < 1){return this;}

    bind();


    auto size = getSize();
    size /= glm::pow(2, level);
    int bytes = size.x * size.y * sizeof(glm::vec4);

    GLubyte* imgData = new GLubyte[bytes];
    glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    unbind();
    ezr::TextureData texData;
    texData.bytesPerPixel = 4;
    texData.data = imgData;
    texData.width = size.x;
    texData.height = size.y;

    return ezr::Texture(texData);
}

std::vector<GLubyte> ezr::Texture::download()
{
    bind();
    int size = _width * _height * sizeof(glm::vec4);
    GLubyte* imgData = new GLubyte[size];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    unbind();
    std::vector<GLubyte> result;
    result.assign(imgData, imgData + size);
    return result;
}

ezr::Texture::~Texture()
{

}
