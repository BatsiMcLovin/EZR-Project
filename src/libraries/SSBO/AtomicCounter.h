//
// Created by rheidrich on 11.07.17.
//

#ifndef EZR_PROJECT_ATOMICCOUNTER_H
#define EZR_PROJECT_ATOMICCOUNTER_H


#include "Utils/debug.h"

namespace ezr
{
    class AtomicCounter
    {
    public:
        AtomicCounter();

        void bind(int bindingPoint = 0);
        void unbind();
        void reset(int content);

        GLuint download();

    private:
        GLuint _handle;
    };
}


#endif //EZR_PROJECT_ATOMICCOUNTER_H
