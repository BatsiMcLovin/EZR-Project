//
// Created by dino on 02.01.16.
//

#ifndef EZR_PROJECT_MLAA_H
#define EZR_PROJECT_MLAA_H

#include "Utils/debug.h"
#include "FBO/FBO.h"
#include "Shaders/SimpleShader.h"
#include "Renderables/Quad.h"

namespace ezr
{
    class MLAA
    {
    public:
        MLAA(GLuint width, GLuint height);
        ~MLAA();

        ezr::Texture* filter(ezr::Texture* tex);

        ezr::Texture* getEdgeTexture();
        ezr::Texture* getPatternTexture();

    private:

        ezr::FBO _fboEdgeDetect;
        ezr::FBO _fboPatternDetection;
        ezr::FBO _fboBlending;

        ezr::Texture* _edgeTex;
        ezr::Texture* _patternTex;
        ezr::Texture* _blendedTex;

        ezr::SimpleShader _mlaa00;
        ezr::SimpleShader _mlaa01;
        ezr::SimpleShader _mlaa02;

        ezr::Texture _lut;
        ezr::Quad _quad;


    };
}


#endif //EZR_PROJECT_MLAA_H
