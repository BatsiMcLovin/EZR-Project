//
// Created by rheidrich on 14.08.17.
//

#ifndef EZR_PROJECT_TEXTUREARRAY_H
#define EZR_PROJECT_TEXTUREARRAY_H


#include "Texture.h"
#include "FBO.h"

namespace ezr
{
    class TextureArray : public ezr::Texture
    {
    public:
        TextureArray(int width, int height, int depth);

        void fillWithFBOContents(ezr::FBO fbo, int num);
        void fillWithTextures(std::vector<ezr::Texture> textures, int level = 0);

    private:
        GLint _depth;

    };
}


#endif //EZR_PROJECT_TEXTUREARRAY_H
