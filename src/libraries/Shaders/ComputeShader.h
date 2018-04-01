//
// Created by rheidrich on 19.05.17.
//

#ifndef EZR_PROJECT_COMPUTESHADER_H
#define EZR_PROJECT_COMPUTESHADER_H


#include "SimpleShader.h"

namespace ezr
{
    class ComputeShader : public ezr::SimpleShader
    {
    public:
        ComputeShader(std::string srcCode);
		void dispatch(GLuint x, GLuint y = 1, GLuint z = 1);
		void memoryBarrier(GLenum memoryBarrierBit = GL_ALL_BARRIER_BITS);
    };
}


#endif //EZR_PROJECT_COMPUTESHADER_H
