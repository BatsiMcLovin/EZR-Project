#ifndef SSBO_H
#define SSBO_H


#include <Utils/debug.h>

namespace ezr
{

class SSBO
{
public:

    SSBO(ezr::SSBO* ssbo);

    template <typename T>
    SSBO(std::vector<T> data, GLint index = 0);
    SSBO(GLuint size, GLint index = 0);
    template <typename T>
    std::vector<T> downloadData();

    void bind(GLint index = 0);

    void unbind();
    void reset(int content = 0);
    void free();

    int getSize();
    GLuint getHandle();

private:

    int mSize = 0;
    GLuint _ssboHandle = 0;

};

}

namespace ezr
{

template<typename T>
SSBO::SSBO(std::vector<T> data, GLint index)
{
    glGenBuffers(1, &_ssboHandle);

    bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * data.size(), &data[0], GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, _ssboHandle);

    mSize = sizeof(T) * data.size();

    unbind();
}

template <typename T>
std::vector<T> SSBO::downloadData()
{

    int dataSize = mSize;
    std::vector<T> output(dataSize / sizeof(T));

    if(output.size() == 0)
    {
        ezr::debug::println("ERROR: Requesting empty SSBO in downloadData() function");
        return output;
    }

    // read data from SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboHandle);
    GLvoid* content = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if(content == NULL)
    {
        ezr::debug::println("ERROR: Requesting empty SSBO in downloadData() function");
        return output;
    }
    memcpy(&output[0], content, dataSize);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return output;
}


}

#endif // SSBO_H
