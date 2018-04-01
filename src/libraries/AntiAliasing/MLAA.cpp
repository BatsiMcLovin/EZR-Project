//
// Created by dino on 02.01.16.
//

#include "MLAA.h"

ezr::MLAA::MLAA(GLuint width, GLuint height) : _fboEdgeDetect(width, height),
                                               _fboPatternDetection(width, height),
                                               _fboBlending(width, height),
                                               _mlaa00(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/AntiAliasing/MLAA/MLAA00.frag"),
                                               _mlaa01(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/AntiAliasing/MLAA/MLAA01.frag"),
                                               _mlaa02(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/AntiAliasing/MLAA/MLAA02.frag"),
                                               _lut(EZR_PROJECT_PATH "/resources/mlaaLUT.png")
{

    //FBO for Edge Detection
    _fboEdgeDetect.generateColorBuffer("edgeDetectOutput", 0);
    _edgeTex = _fboEdgeDetect.getTexturePointer("edgeDetectOutput");
    _fboEdgeDetect.generateDepthBuffer("depthOutput");
    _fboEdgeDetect.compile();

    //FBO for Pattern Detection
    _fboPatternDetection.generateColorBuffer("patternDetectOutput", 0);
    _patternTex = _fboPatternDetection.getTexturePointer("patternDetectOutput");
    _fboPatternDetection.generateDepthBuffer("depthOutput");
    _fboPatternDetection.compile();

    //FBO for Blending
    _fboBlending.generateColorBuffer("blendedOutput", 0);
    _blendedTex = _fboBlending.getTexturePointer("blendedOutput");
    _fboBlending.generateDepthBuffer("depthOutput");
    _fboBlending.compile();
}

ezr::MLAA::~MLAA()
{

}

ezr::Texture *ezr::MLAA::filter(ezr::Texture *tex)
{
    //Edge Detection

    _mlaa00.bind();

    _fboEdgeDetect.bind();

    _mlaa00.uploadUniform(tex, "tex", 0);
    _quad.draw();

    _fboEdgeDetect.unbind();

    //Pattern Detection

    _mlaa01.bind();

    _fboPatternDetection.bind();

    _mlaa01.uploadUniform(_edgeTex, "tex", 0);
    _mlaa01.uploadUniform(_lut, "lut", 1);
    _quad.draw();

    _fboPatternDetection.unbind();

    //Blending

    _mlaa02.bind();

    _fboBlending.bind();

    _mlaa02.uploadUniform(tex, "tex", 0);
    _mlaa02.uploadUniform(_patternTex, "edges", 1);
    _quad.draw();

    _fboBlending.unbind();

    return _blendedTex;
}

ezr::Texture *ezr::MLAA::getEdgeTexture()
{
    return _edgeTex;
}

ezr::Texture *ezr::MLAA::getPatternTexture()
{
    return _patternTex;
}
