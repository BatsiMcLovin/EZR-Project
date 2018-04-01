//
// Created by rheidrich on 03.03.17.
//

#ifndef EZR_PROJECT_SHADERUTILS_H
#define EZR_PROJECT_SHADERUTILS_H

#include <Renderables/Renderable.h>
#include <FBO/FBO.h>
#include <Shaders/ComputeShader.h>
#include "Shaders/SimpleShader.h"

namespace ezr
{
    enum GBufferOutput
    {
        DIFFUSE_OUTPUT,
        SPECULAR_OUTPUT,
        EMISSION_OUTPUT,
        NORMALS_OUTPUT,
        MATERIAL_ID_OUTPUT,
        POSITION_OUTPUT,
        DEPTH_OUTPUT
    };

    class ShaderUtils
    {

    public:
        static SimpleShader loadFromPaths(std::string vertexShaderPath, std::string fragmentShaderPath);

        static SimpleShader loadFromPaths(std::string vertexShaderPath,
                                          std::string geometryShaderPath,
                                          std::string fragmentShaderPath);

        static SimpleShader loadFromCode(std::string vertexShaderCode, std::string fragmentShaderCode);

        static SimpleShader loadFromCode(std::string vertexShaderCode,
                                         std::string geometryShaderCode,
                                         std::string fragmentShaderCode);
        static ComputeShader loadComputeShaderFromPath(std::string path);

        static std::vector<SimpleShader> createGBuffers(std::vector<ezr::Renderable*> renderables);
        static SimpleShader createGBuffer(ezr::Renderable renderable);

        static std::map<GBufferOutput, Texture> createGBufferFBOTextures(FBO* fbo);

        static void uploadGBufferMaterialUniforms(SimpleShader shader, Renderable renderable);

    };
}


#endif //EZR_PROJECT_SHADERUTILS_H
