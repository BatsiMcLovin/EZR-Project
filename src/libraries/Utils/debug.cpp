//
// Created by dino on 04.11.15.
//

#include "debug.h"

void ezr::debug::print(std::string s)
{

    std::cout << s << std::flush;

}

void ezr::debug::println(std::string s)
{

    ezr::debug::print(s + "\n");

}

void ::ezr::debug::print(glm::mat4 mat)
{

    ezr::debug::print(ezr::debug::to_string(mat));

}

void ::ezr::debug::println(glm::mat4 mat)
{

    ezr::debug::println(ezr::debug::to_string(mat));

}

void ::ezr::debug::println(glm::mat3 mat)
{
    ezr::debug::println(ezr::debug::to_string(mat));
}

void ::ezr::debug::print(glm::mat3 mat)
{
    ezr::debug::print(ezr::debug::to_string(mat));
}

std::string ezr::debug::to_string(glm::mat3 mat)
{

    std::string matString;
    matString += "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + " )\n";
    matString += "(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + " )\n";
    matString += "(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + " )\n";

    return matString;
}

std::string ezr::debug::to_string(glm::mat4 mat)
{

    std::string matString;
    matString += "(" + std::to_string(mat[0][0]) + ", " + std::to_string(mat[1][0]) + ", " + std::to_string(mat[2][0]) + ", " + std::to_string(mat[3][0]) + " )\n";
    matString += "(" + std::to_string(mat[0][1]) + ", " + std::to_string(mat[1][1]) + ", " + std::to_string(mat[2][1]) + ", " + std::to_string(mat[3][1]) + " )\n";
    matString += "(" + std::to_string(mat[0][2]) + ", " + std::to_string(mat[1][2]) + ", " + std::to_string(mat[2][2]) + ", " + std::to_string(mat[3][2]) + " )\n";
    matString += "(" + std::to_string(mat[0][3]) + ", " + std::to_string(mat[1][3]) + ", " + std::to_string(mat[2][3]) + ", " + std::to_string(mat[3][3]) + " )\n";

    return matString;
}

void ::ezr::debug::println(float val)
{
    ezr::debug::println(std::to_string(val));
}

void ::ezr::debug::print(float val)
{
    ezr::debug::print(std::to_string(val));
}

void ::ezr::debug::print(glm::vec4 vec)
{
    ezr::debug::print(ezr::debug::to_string(vec));
}

void ::ezr::debug::println(glm::vec4 vec)
{
    ezr::debug::print(ezr::debug::to_string(vec) + "\n");
}

void ::ezr::debug::print(glm::vec3 vec)
{
    ezr::debug::print(ezr::debug::to_string(vec));
}

void ::ezr::debug::println(glm::vec3 vec)
{
    ezr::debug::print(ezr::debug::to_string(vec) + "\n");
}

void ezr::debug::print(glm::vec2 vec)
{
    ezr::debug::print(ezr::debug::to_string(vec));
}

void ezr::debug::println(glm::vec2 vec)
{
    ezr::debug::print(ezr::debug::to_string(vec) + "\n");
}

std::string ezr::debug::to_string(glm::vec4 vec)
{
    return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.a) + "]^T ";
}

std::string ezr::debug::to_string(glm::vec3 vec)
{
    return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) +  "]^T ";
}

std::string ezr::debug::to_string(glm::vec2 vec)
{
    return "[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + "]^T ";
}

std::string ezr::debug::to_string(float value, int precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string ezr::debug::to_string(int value)
{
    std::stringstream ss;
    ss << std::fixed << value;
    return ss.str();
}

std::string ezr::debug::glError()
{
    GLenum error = glGetError();

    switch (error)
    {
        case GL_INVALID_VALUE:
            return std::string("GL_INVALID_VALUE");
        case GL_INVALID_ENUM:
            return std::string("GL_INVALID_ENUM");
        case GL_INVALID_OPERATION:
            return std::string("GL_INVALID_OPERATION");
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return std::string("GL_INVALID_FRAMEBUFFER_OPERATION");
        case GL_OUT_OF_MEMORY:
            return std::string("GL_OUT_OF_MEMORY");
        case GL_NO_ERROR:
            return std::string("GL_NO_ERROR");
        default:
            return std::string();
    }
}

void ::ezr::debug::glErrorPrintln()
{
    println(glError());
}
