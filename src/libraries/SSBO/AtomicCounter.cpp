//
// Created by rheidrich on 11.07.17.
//

#include "AtomicCounter.h"

ezr::AtomicCounter::AtomicCounter()
{
    glGenBuffers(1, &_handle);

    reset(0);
}

void ezr::AtomicCounter::bind(int bindingPoint)
{
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, bindingPoint, _handle);
}
void ezr::AtomicCounter::unbind()
{
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void ezr::AtomicCounter::reset(int content)
{
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _handle);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &content, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

GLuint ezr::AtomicCounter::download()
{
    // read data from SSBO
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _handle);
    GLuint* content = (GLuint*) glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);

    return content[0];
}


