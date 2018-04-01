//
// Created by dino on 04.12.15.
//

#ifndef EZR_PROJECT_FBO_H
#define EZR_PROJECT_FBO_H

#include "Utils/debug.h"
#include "Texture.h"



namespace ezr
{
    class FBO
    {
    public:
        FBO(int width, int height, GLenum format = GL_RGBA16F);
        ~FBO();

        void bind(GLenum framebufferType = GL_FRAMEBUFFER);
        void unbind(GLenum framebufferType = GL_FRAMEBUFFER);

        void compile();

        void blit();

        Texture generateColorBuffer(std::string name, GLuint colorAttachementIndex);
        Texture generateColorBuffer(std::string name, GLuint colorAttachementIndex, int width, int height);
        Texture generateDepthBuffer(std::string name, int width, int height);
        Texture generateDepthBuffer(std::string name = "depth");

        GLuint getHandle();
        ezr::Texture* getTexturePointer(std::string name);

        bool hasTexture(std::string name);

        void free(bool freeRenderBuffers = true);

    private:
        std::map<std::string, ezr::Texture> _buffers;
        std::vector<GLenum> _colorAttachments;

        GLint _width, _height;

        GLuint _handle;

        GLenum _format = GL_RGBA16F;
    };
}


#endif //EZR_PROJECT_FBO_H
