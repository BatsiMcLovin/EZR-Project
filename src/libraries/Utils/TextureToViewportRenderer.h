//
// Created by rheidrich on 14.06.17.
//

#ifndef EZR_PROJECT_TEXTURETOVIEWPORTRENDERER_H
#define EZR_PROJECT_TEXTURETOVIEWPORTRENDERER_H


#include <Renderables/Quad.h>
#include <Shaders/SimpleShader.h>
#include <FBO/TextureArray.h>

namespace ezr
{
    class TextureToViewportRenderer
    {
    public:
        TextureToViewportRenderer();
        ~TextureToViewportRenderer();
		void renderTexture(ezr::TextureArray texture, int layer = 0);
		void renderTexture(ezr::Texture texture);
		void renderTexture(ezr::Texture* texture);


    private:
        ezr::Quad _quad;
        ezr::SimpleShader _quadShader2D;
        ezr::SimpleShader _quadShader2DArray;
    };
}


#endif //EZR_PROJECT_TEXTURETOVIEWPORTRENDERER_H
