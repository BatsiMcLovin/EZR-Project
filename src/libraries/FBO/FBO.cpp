//
// Created by dino on 04.12.15.
//

#include "FBO.h"

ezr::FBO::FBO(int width, int height, GLenum format)
{
    glEnable(GL_MULTISAMPLE);

    _width = width;
    _height = height;
    _format = format;

    glGenFramebuffers(1, &_handle);
}

ezr::FBO::~FBO()
{

}

ezr::Texture ezr::FBO::generateColorBuffer(std::string name, GLuint colorAttachementIndex)
{
    return generateColorBuffer(name, colorAttachementIndex, _width, _height);
}

ezr::Texture ezr::FBO::generateColorBuffer(std::string name, GLuint colorAttachementIndex, int width, int height)
{
    bind();

    GLuint texHandle;

    GLenum target = GL_TEXTURE_2D;
    colorAttachementIndex = GL_COLOR_ATTACHMENT0 + colorAttachementIndex;

    glGenTextures(1, &texHandle);
    glBindTexture(target, texHandle);
//    glEnable(target);
    glTexImage2D(target, 0, _format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glGenerateMipmap(target);
//    glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachementIndex, target, texHandle, 0);

    ezr::Texture tex(texHandle, (GLuint) width, (GLuint) height);
    tex.setTarget(target);

    _colorAttachments.push_back(colorAttachementIndex);
    _buffers.insert(std::pair<std::string, Texture>(name, tex) );

    unbind();

    return tex;
}

ezr::Texture ezr::FBO::generateDepthBuffer(std::string name)
{
    return generateDepthBuffer(name, _width, _height);
}

ezr::Texture ezr::FBO::generateDepthBuffer(std::string name, int width, int height)
{
    GLuint depthtextureHandle;

    bind();

    glGenTextures(1, &depthtextureHandle);
    glBindTexture(GL_TEXTURE_2D, depthtextureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_BYTE, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthtextureHandle, 0);

    unbind();

    ezr::Texture tex(depthtextureHandle, (GLuint) width, (GLuint) height);
    _buffers.insert(std::pair<std::string, ezr::Texture>(name, tex) );

    return tex;
}


void ezr::FBO::bind(GLenum framebufferType)
{
    glBindFramebuffer(framebufferType, _handle);
}

void ezr::FBO::unbind(GLenum framebufferType)
{
    glBindFramebuffer(framebufferType, 0);
}

void ezr::FBO::compile()
{
    bind();
    glDrawBuffers(_colorAttachments.size(), &_colorAttachments[0]);
    unbind();
}

void ezr::FBO::blit()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _handle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Normal FBO can be the default FBO too.
    glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

}

GLuint ezr::FBO::getHandle()
{
    return _handle;
}

bool ezr::FBO::hasTexture(std::string name)
{
    std::map<std::string, ezr::Texture>::iterator iter = _buffers.find(name);

    return iter != _buffers.end();
}

ezr::Texture* ezr::FBO::getTexturePointer(std::string name)
{
    if(hasTexture(name))
    {
        return &_buffers.at(name);
    }
    else
        return 0;
}

void ezr::FBO::free(bool freeRenderBuffers)
{

    glDeleteFramebuffers(1, &_handle);
    if(freeRenderBuffers)
    {
        for(auto value : _buffers)
        {
            GLuint texHandle = value.second.getTexHandle();
            glDeleteTextures(1, &texHandle);
        }
    }


}
