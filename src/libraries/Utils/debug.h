//
// Created by dino on 04.11.15.
//

#ifndef EZR_PROJECT_DEBUG_H
#define EZR_PROJECT_DEBUG_H

#define GLM_FORCE_RADIANS

#ifdef _WIN32
#define M_PI 3.14159265359
#endif





#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif // !M_PI_2


//C++ stuff
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "vector"
#include "fstream"
#include "map"
#include "functional"
#include "exception"

//GLFW & GLEW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//GLM
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/common.hpp>

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

//IMGUI
#include <IMGUI/imgui.h>
#include "IMGUI/imgui_impl_glfw_gl3.h"

namespace ezr
{
    namespace debug
    {
        void print(std::string s);
        void println(std::string s);

        void print(glm::mat4 mat);
        void println(glm::mat4 mat);

        void print(glm::mat3 mat);
        void println(glm::mat3 mat);

        void print(glm::vec4 vec);
        void println(glm::vec4 vec);

        void print(glm::vec3 vec);
        void println(glm::vec3 vec);

        void print(glm::vec2 vec);
        void println(glm::vec2 vec);

        void print(float val);
        void println(float val);

        std::string to_string(glm::mat4 mat);
        std::string to_string(glm::mat3 mat);
        std::string to_string(glm::vec4 vec);
        std::string to_string(glm::vec3 vec);
        std::string to_string(glm::vec2 vec);
        std::string to_string(float value, int precision = 4);
        std::string to_string(int value);

        std::string glError();
        void glErrorPrintln();
    }
}


#endif //EZR_PROJECT_DEBUG_H
