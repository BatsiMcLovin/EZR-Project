//
// Created by dino on 02.12.15.
//

#ifndef EZR_PROJECT_SHADER_H
#define EZR_PROJECT_SHADER_H

#include "Utils/debug.h"
#include "Utils/paths.h"
#include "FBO/Texture.h"
#include "FBO/Cubemap.h"

namespace ezr
{
    enum ShaderType
    {
        EZR_VERTEX_SHADER_PATH,
        EZR_FRAGMENT_SHADER_PATH,
        EZR_GEOMETRY_SHADER_PATH,
        EZR_TESSELATION_SHADER_PATH,
        EZR_VERTEX_SHADER_CODE,
        EZR_FRAGMENT_SHADER_CODE,
        EZR_GEOMETRY_SHADER_CODE,
        EZR_TESSELATION_SHADER_CODE
    };

    class SimpleShader
    {
    public:
        SimpleShader();

        SimpleShader(std::map<ShaderType, std::string> shaderMap);
        SimpleShader(std::string vertexShaderPath, std::string fragShaderPath);
        SimpleShader(std::string vertexShaderPath, std::string geomShaderPath, std::string fragShaderPath);
        bool compile(std::string code, GLenum type);
        bool linkShadersToProgramHandle();

        void bind();
        bool isOK();

        bool uploadUniform(glm::mat4 mat, std::string name);
        bool uploadUniform(glm::vec4 vec, std::string name);
        bool uploadUniform(glm::vec3 vec, std::string name);
        bool uploadUniform(glm::vec2 vec, std::string name);
        bool uploadUniform(float val, std::string name);
        bool uploadUniform(int val, std::string name);
        bool uploadUniform(bool val, std::string name);
        bool uploadUniform(Texture tex, std::string name, GLuint texSlot = 0);
        bool uploadUniform(Texture* tex, std::string name, GLuint texSlot = 0);
		bool uploadUniform(ezr::Cubemap cubemap, std::string name, GLuint texSlot = 0);

        GLuint getProgramHandle();

        GLint getUniformLocation(std::string name);
        bool hasUniform(std::string name);

        void readUniforms();


        void free();

    protected:
        std::string readMap(std::map<ShaderType, std::string> shaderMap, ShaderType pathType, ShaderType codeType);


        GLuint _programHandle;
        std::vector<GLuint> _shaderHandles;
        std::map<std::string, GLint> _uniformLocations;
        bool _isOk;


    };

}


#endif //EZR_PROJECT_SHADER_H
