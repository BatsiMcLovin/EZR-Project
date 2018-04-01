//
// Created by dino on 02.12.15.
//

#include "SimpleShader.h"


ezr::SimpleShader::SimpleShader()
{

}

ezr::SimpleShader::SimpleShader(std::string vert, std::string frag)
{
    _programHandle = glCreateProgram();


    try {
        bool v_success = compile(ezr::paths::loadFileWithLog(vert), GL_VERTEX_SHADER);
        bool f_success = compile(ezr::paths::loadFileWithLog(frag), GL_FRAGMENT_SHADER);

        if(v_success && f_success)
        {
            linkShadersToProgramHandle();
        }

    }catch (std::exception e)
    {
        ezr::debug::println("FAILURE : Loading these Shaders was not a success *sadface* ");
    }
}



ezr::SimpleShader::SimpleShader(std::string vert, std::string geom, std::string frag)
{
    _programHandle = glCreateProgram();

    ezr::debug::println(vert);
    bool v_success = compile(ezr::paths::loadFileWithLog(vert), GL_VERTEX_SHADER);
    bool g_success = compile(ezr::paths::loadFileWithLog(geom), GL_GEOMETRY_SHADER);
    bool f_success = compile(ezr::paths::loadFileWithLog(frag), GL_FRAGMENT_SHADER);

    if(v_success && f_success && g_success)
    {
        linkShadersToProgramHandle();
    }
    else
    {
        ezr::debug::println("FAILURE : Loading these Shaders was not a success *sadface* ");
    }

}

ezr::SimpleShader::SimpleShader(std::map<ezr::ShaderType, std::string> shaderMap)
{
    _programHandle = glCreateProgram();

    try {
        std::string vert = (shaderMap.find(EZR_VERTEX_SHADER_PATH) != shaderMap.end()) ?
                           ezr::paths::loadFileWithLog(shaderMap[EZR_VERTEX_SHADER_PATH]) :
                           shaderMap[EZR_VERTEX_SHADER_CODE];
        compile(vert, GL_VERTEX_SHADER);

        std::string geom = (shaderMap.find(EZR_GEOMETRY_SHADER_PATH) != shaderMap.end()) ?
                           ezr::paths::loadFileWithLog(shaderMap[EZR_GEOMETRY_SHADER_PATH]) :
                           shaderMap[EZR_GEOMETRY_SHADER_CODE];
        if(geom.length() > 0)
            compile(geom, GL_GEOMETRY_SHADER);

        std::string frag = (shaderMap.find(EZR_FRAGMENT_SHADER_PATH) != shaderMap.end()) ?
                           ezr::paths::loadFileWithLog(shaderMap[EZR_FRAGMENT_SHADER_PATH]) :
                           shaderMap[EZR_FRAGMENT_SHADER_CODE];
        compile(frag, GL_FRAGMENT_SHADER);
    }catch (std::exception e)
    {
        ezr::debug::println(e.what());
        return;
    }

    linkShadersToProgramHandle();
}

bool ezr::SimpleShader::compile(std::string code, GLenum type)
{
    // Shader erstellen
    GLuint shaderHandle = glCreateShader(type);

    const char * code_str = code.c_str();
    // Code dem ShaderHandle zuweisen
    glShaderSource(shaderHandle, 1, &code_str, 0);
    // Shader bauen
    glCompileShader(shaderHandle);

    GLint isCompiled = 0;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::string errorLog;
        errorLog.resize(1000);
        glGetShaderInfoLog(shaderHandle, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(shaderHandle); // Don't leak the shader.

        ezr::debug::println("ERROR : " + errorLog);

    } else
        _shaderHandles.push_back(shaderHandle);


    return isCompiled;
}

bool ezr::SimpleShader::linkShadersToProgramHandle()
{
    ezr::debug::println("SUCCESS : Loading these Shaders was a success");

    for (GLuint handle : _shaderHandles)
    {
        glAttachShader(_programHandle, handle);
    }
    glLinkProgram(_programHandle);

    GLint linkStatus;
    glGetProgramiv(_programHandle, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        GLchar log[255];
        int length = -1;
        glGetProgramInfoLog(_programHandle, 255, &length, log);

        ezr::debug::println(log);
        ezr::debug::println("FAILURE : Shaders were not successfully linked. Hasta la vista.");
        glfwTerminate();
        _isOk = false;
    }
    else
    {
        ezr::debug::println("SUCCESS : Shaders were successfully linked into a Program");
        _isOk = true;
    }

    return _isOk;
}

void ezr::SimpleShader::bind()
{
    glUseProgram(_programHandle);
}

bool ezr::SimpleShader::isOK()
{
    return _isOk;
}
GLuint ezr::SimpleShader::getProgramHandle()
{
    return _programHandle;
}

bool ezr::SimpleShader::uploadUniform(glm::mat4 mat, std::string name)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        return true;
    }
    return false;
}

bool ezr::SimpleShader::uploadUniform(glm::vec4 vec, std::string name)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);
        glUniform4fv(location, 1, glm::value_ptr(vec));
        return true;
    }
    return false;
}

bool ezr::SimpleShader::uploadUniform(glm::vec3 vec, std::string name)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);
        glUniform3fv(location, 1, glm::value_ptr(vec));
        return true;
    }
    return false;
}

bool ezr::SimpleShader::uploadUniform(glm::vec2 vec, std::string name)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);
        glUniform2fv(location, 1, glm::value_ptr(vec));
        return true;
    }
    return false;
}

bool ezr::SimpleShader::uploadUniform(float val, std::string name)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);
        glUniform1f(location, val);
        return true;
    }

    return false;
}

bool ezr::SimpleShader::uploadUniform(int val, std::string name)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);
        glUniform1i(location, val);
        return true;
    }
    return false;
}

bool ezr::SimpleShader::uploadUniform(bool val, std::string name)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);
        glUniform1i(location, val);
        return true;
    }
    return false;
}


bool ezr::SimpleShader::uploadUniform(ezr::Texture tex, std::string name, GLuint texSlot)
{
    return uploadUniform(&tex, name, texSlot);
}



bool ezr::SimpleShader::uploadUniform(ezr::Texture *tex, std::string name, GLuint texSlot)
{
    if(hasUniform(name))
    {
        GLint location = getUniformLocation(name);

        glActiveTexture(GL_TEXTURE0 + texSlot);
        glEnable(GL_TEXTURE_2D);
        tex->bind();
        glUniform1i(location, texSlot);

        glActiveTexture(GL_TEXTURE0);

        return true;
    }

    return false;
}

bool ezr::SimpleShader::uploadUniform(ezr::Cubemap cubemap, std::string name, GLuint texSlot)
{
	/*
	*/
	if (hasUniform(name))
	{
		GLint location = getUniformLocation(name);

		glActiveTexture(GL_TEXTURE0 + texSlot);
		//glEnable(GL_TEXTURE_CUBE_MAP);
		cubemap.bind();
		glUniform1i(location, texSlot);

		glActiveTexture(GL_TEXTURE0);

		return true;
	}
	return false;
}

GLint ezr::SimpleShader::getUniformLocation(std::string name)
{
    if(_uniformLocations.count(name) > 0)
        return _uniformLocations[name];
    else
        return -1;
}

bool ezr::SimpleShader::hasUniform(std::string name)
{
    bool inMap = _uniformLocations.count(name) > 0;

    if(!inMap)
    {
        GLint location = glGetUniformLocation(_programHandle, name.c_str());

        _uniformLocations.insert(std::pair<std::string, GLint>(name, location));
        _uniformLocations[name] = location;
    }

    return getUniformLocation(name) != -1;
}

void ezr::SimpleShader::readUniforms()
{
    GLint num = 0;
    glGetProgramiv(_programHandle, GL_ACTIVE_UNIFORMS, &num);

    ezr::debug::println(std::to_string(num));

    for (int i = 0; i < num; i++)
    {
        //passive variables for glGetActiveUniform
        int nameLength=-1;
        int uniformSize=-1;
        GLenum type = GL_ZERO;
        //string that saves uniformName
        char uniformName[50];
        glGetActiveUniform(_programHandle, GLint(i), sizeof(uniformName)-1, &nameLength, &uniformSize, &type, uniformName);
        uniformName[nameLength] = 0;
        //add uniform variable to map
        ezr::debug::println(uniformName);
    }

}

std::string ezr::SimpleShader::readMap(std::map<ezr::ShaderType, std::string> shaderMap,
                                       ezr::ShaderType pathType,
                                       ezr::ShaderType codeType)
{
    std::string code = (shaderMap.find(pathType) != shaderMap.end()) ?
                        ezr::paths::loadFileWithLog(shaderMap[pathType]) :
                       (shaderMap.find(codeType) != shaderMap.end() ? shaderMap[codeType] : "");

    return code;
}

void ezr::SimpleShader::free()
{
    glUseProgram(0);
    glDeleteProgram(_programHandle);
}

