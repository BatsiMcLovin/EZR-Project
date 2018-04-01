#include "SSBO.h"

ezr::SSBO::SSBO(ezr::SSBO* ssbo)
{
    if(ssbo == NULL) { return; }
    mSize = ssbo->getSize();
    _ssboHandle = ssbo->getHandle();
}

ezr::SSBO::SSBO(GLuint size, GLint index)
{
    glGenBuffers(1, &_ssboHandle);
//    glCreateBuffers(1, &_ssboHandle);

    bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, 0, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, _ssboHandle);

    mSize = size;

    unbind();
}

void ezr::SSBO::bind(GLint index)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, _ssboHandle);
}

void ezr::SSBO::unbind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ezr::SSBO::free()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboHandle);
    bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, 0, 0, 0);
    glDeleteBuffers(1, &_ssboHandle);
}

GLuint ezr::SSBO::getHandle()
{
    return _ssboHandle;
}

int ezr::SSBO::getSize()
{
    return mSize;
}

void ezr::SSBO::reset(int content)
{
    std::vector<glm::vec4> data;
    data.resize(mSize);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboHandle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mSize, &data[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}