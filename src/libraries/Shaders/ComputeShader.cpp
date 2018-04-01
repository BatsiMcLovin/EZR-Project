//
// Created by rheidrich on 19.05.17.
//

#include <Utils/debug.h>
#include "ComputeShader.h"

ezr::ComputeShader::ComputeShader(std::string srcCode)
{
    ezr::debug::println("Creating Compute Shader Program");
    _programHandle = glCreateProgram();
    compile(srcCode, GL_COMPUTE_SHADER);
    linkShadersToProgramHandle();
}

void ezr::ComputeShader::dispatch(GLuint x, GLuint y, GLuint z)
{
	glDispatchCompute(x, y, z);
}

void ezr::ComputeShader::memoryBarrier(GLenum memoryBarrierBit)
{
	glMemoryBarrier(memoryBarrierBit);
}
