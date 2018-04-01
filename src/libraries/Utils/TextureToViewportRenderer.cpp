//
// Created by rheidrich on 14.06.17.
//

#include "TextureToViewportRenderer.h"
#include "ShaderUtils.h"

ezr::TextureToViewportRenderer::TextureToViewportRenderer()
{
    _quadShader2D = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                 EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");
    _quadShader2DArray = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                 EZR_SHADER_PATH "/GBuffer/simpleTextureArray.frag");
}

void ezr::TextureToViewportRenderer::renderTexture(ezr::Texture texture)
{
    _quadShader2D.bind();
    _quadShader2D.uploadUniform(texture, "tex");
    _quad.draw();
}

void ezr::TextureToViewportRenderer::renderTexture(ezr::Texture* texture)
{
	renderTexture(*texture);
}

ezr::TextureToViewportRenderer::~TextureToViewportRenderer()
{
    _quad.free();
    _quadShader2D.free();
    _quadShader2DArray.free();
}

void ezr::TextureToViewportRenderer::renderTexture(ezr::TextureArray texture, int layer)
{
    _quadShader2DArray.bind();
    _quadShader2DArray.uploadUniform(texture, "tex");
    _quadShader2DArray.uploadUniform(layer, "layer");
    _quad.draw();
}
