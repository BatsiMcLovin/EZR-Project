#ifndef EZR_PROJECT_TEXTURE_H
#define EZR_PROJECT_TEXTURE_H

//stb image
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#include "Utils/debug.h"

namespace ezr
{

	struct TextureData
	{
        TextureData(){}
        TextureData(unsigned char *d, int w, int h, int b)
        {
            data = d;
            width = w;
            height = h;
            bytesPerPixel = b;
        }
		unsigned char *data;
		GLint width;
		GLint height; 
		GLint bytesPerPixel;
	};

    class Texture
    {
    public:
        Texture();
        virtual ~Texture();
        Texture(ezr::Texture* texture);
        Texture(std::string path);
		Texture(ezr::TextureData texData);
        Texture(GLuint handle, GLuint width = 0, GLuint height = 0);

        void bind();
        void unbind();

		void bindImageTexture(GLuint unit, GLenum access = GL_READ_WRITE, GLenum format = GL_RGBA8);

        GLuint getTexHandle();

        void setTarget(GLenum target);

        glm::ivec2 getSize();

        void free();

        ezr::Texture getMipMapTexture(int level);
        std::vector<GLubyte> download();

    protected:
		void generateTexture(ezr::TextureData texData);

        int _width, _height, _bytesPerPixel;
        GLuint _handle;
        GLenum _target;
    };
}


#endif //EZR_PROJECT_TEXTURE_H
