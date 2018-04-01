//
// Created by rheidrich on 14.08.17.
//

#include "TextureArray.h"

ezr::TextureArray::TextureArray(int width, int height, int depth)
{

    _width = width;
    _height = height;
    _depth = depth;
    _bytesPerPixel = 4;
    _target = GL_TEXTURE_2D_ARRAY;
    glGenTextures(1, &_handle);
    glBindTexture(GL_TEXTURE_2D_ARRAY, _handle);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 5, GL_RGBA8, width, height, depth);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ezr::TextureArray::fillWithFBOContents(ezr::FBO fbo, int num)
{
    fbo.bind(GL_READ_FRAMEBUFFER);
    glBindTexture(GL_TEXTURE_2D_ARRAY, _handle);

    for (int i = 0; i < num; ++i)
    {
        glReadBuffer((GLenum) (GL_COLOR_ATTACHMENT0 + i));
        glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 0, 0, _width, _height);
    }
    fbo.unbind(GL_READ_FRAMEBUFFER);

}

void ezr::TextureArray::fillWithTextures(std::vector<ezr::Texture> textures, int level)
{
    for (int i = 0; i < textures.size(); ++i)
    {
        auto tex = textures[i];
        auto srcSize = tex.getSize() / glm::pow(2, level);

        tex.bind();
        glCopyImageSubData(tex.getTexHandle(), GL_TEXTURE_2D, level, 0, 0 ,0, _handle, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, srcSize.x, srcSize.y, 1);
    }
}
