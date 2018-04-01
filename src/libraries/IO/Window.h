//
// Created by dino on 16.11.15.
//

#ifndef EZR_PROJECT_WINDOW_H
#define EZR_PROJECT_WINDOW_H

#include "Utils/debug.h"
#include "Camera.h"

namespace ezr
{
    class Window
    {
    public:
        Window(std::string name, int width = 200, int height = 200, int posX = 10, int posY = 10, bool coreProfile = false);
        ~Window();

        bool shouldClose();

        void swapBuffer(); //
        void clear(glm::vec3 color = glm::vec3(0.0));
        void clearDepthBuffer();
        void clearColorBuffer();


        void setCam(ezr::Camera* cam);

        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void mouseCallback(GLFWwindow* win, double x, double y);
        void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods);

        void enableMouseNavigation(bool val);


        void enableDepth(bool val);
        void enableViewports(bool val);
        void useViewport(GLuint viewport);

        glm::vec2 getCursorPosition();
        glm::ivec2 getWindowSize();

        GLFWwindow* getWindow();

    private:
        GLFWwindow* _window;
        ezr::Camera* _cam;

        GLint _width, _height;

        bool _rotateCam;
        bool _useMouseNavigation = true;
        float _camSpeed;
        glm::vec2 _latestMousePos;
        glm::vec2 _secondLatestMousePos;
        bool _useViewports;
    };

}

#endif //EZR_PROJECT_WINDOW_H
