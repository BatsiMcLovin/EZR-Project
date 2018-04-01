//
// Created by dino on 16.11.15.
//

#include "Window.h"

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

ezr::Window::Window(std::string name, int width, int height, int posX, int posY, bool coreProfile)
{

    ezr::debug::println("Initializing GLFW");

    if (!glfwInit())
        exit(EXIT_FAILURE);

    ezr::debug::println("Initializing GLEW");


    if(coreProfile){
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        glewExperimental = GL_TRUE;
    }
    _window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);


    if (!_window)
    {
        ezr::debug::println("ERROR : Something went wrong, while creating a window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowPos(_window, posX, posY);
    glfwSetWindowSize(_window, width, height);
    glfwMakeContextCurrent(_window);

    glewInit();

    printf("OPENGL : %s \n", glGetString(GL_VERSION));

    //glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    static std::function<void (GLFWwindow*,int,int,int,int)> f0 = std::bind(&Window::keyCallback,
                                                                            this,
                                                                            std::placeholders::_1,
                                                                            std::placeholders::_2,
                                                                            std::placeholders::_3,
                                                                            std::placeholders::_4,
                                                                            std::placeholders::_5);

    glfwSetKeyCallback(_window, [] (GLFWwindow* w, int k, int s, int a, int m)
    {
        f0(w, k, s, a, m);
    });

    static std::function<void (GLFWwindow*, double, double)> f1 = std::bind(&Window::mouseCallback,
                                                                            this,
                                                                            std::placeholders::_1,
                                                                            std::placeholders::_2,
                                                                            std::placeholders::_3);

    glfwSetCursorPosCallback(_window, [] (GLFWwindow* w, double x, double y)
    {
        f1(w, x, y);
    });

    static std::function<void (GLFWwindow*, int, int, int)> f2 = std::bind(&Window::mouseButtonCallback,
                                                                           this,
                                                                           std::placeholders::_1,
                                                                           std::placeholders::_2,
                                                                           std::placeholders::_3,
                                                                           std::placeholders::_4);

    glfwSetMouseButtonCallback(_window, [] (GLFWwindow* w, int a, int b, int c)
    {
        f2(w, a, b, c);
    });

    _cam = 0;
    _rotateCam = false;
    _camSpeed = 0.25f;
    _width = width;
    _height = height;

}

ezr::Window::~Window()
{
    glfwDestroyWindow(_window);
}

bool ezr::Window::shouldClose()
{

    return (glfwWindowShouldClose(_window) || glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS);

}

void ezr::Window::swapBuffer()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();
    useViewport(0);
}

void ezr::Window::clear(glm::vec3 color)
{
    glClearColor(color.x, color.y, color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ezr::Window::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ezr::Window::clearColorBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void ezr::Window::setCam(ezr::Camera *cam)
{
    _cam = cam;

    std::string cam_manual = "SUCCESS : Camera has been added. \n\n"
            "##############\n"
            "CAMERA MANUAL\n"
            "______________\n"
            "WASD - Regular Control\n"
            "QE - UP/DOWN\n"
            "R - Reset Camera to origin\n"
            "UP/DOWN - Regulate Camera Speed\n"
            "##############\n\n";
    ezr::debug::print(cam_manual);

}

void ezr::Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }else if(_cam == 0 )
    {
        return;
    }else if(key == GLFW_KEY_W )
    {
        _cam->setRelativePosition(glm::vec3(0.0f, 0.0f, -_camSpeed));
    }else if(key == GLFW_KEY_A)
    {
        _cam->setRelativePosition(glm::vec3(-_camSpeed, 0.0f, 0.0));
    }else if(key == GLFW_KEY_S)
    {
        _cam->setRelativePosition(glm::vec3(0.0f, 0.0f, _camSpeed));
    }else if(key == GLFW_KEY_D)
    {
        _cam->setRelativePosition(glm::vec3(_camSpeed, 0.0f, 0.0));
    }else if(key == GLFW_KEY_Q)
    {
        _cam->setRelativePosition(glm::vec3(0.0f, _camSpeed, 0.0));
    }else if(key == GLFW_KEY_E)
    {
        _cam->setRelativePosition(glm::vec3(0.0f, -_camSpeed, 0.0));
    }else if(key == GLFW_KEY_R)
    {
        _cam->setPosition(glm::vec3(0.0f, 0.0f, 0.0));
        _cam->setOrientation(0.0f, 0.0f, 0.0);
    }else if(key == GLFW_KEY_UP)
    {
        _camSpeed = glm::clamp(_camSpeed + 0.0125f, 0.0125f, 2.0f);
    }else if(key == GLFW_KEY_DOWN)
    {
        _camSpeed = glm::clamp(_camSpeed - 0.0125f, 0.0125f, 2.0f);
    }

}

void ezr::Window::mouseCallback(GLFWwindow *win, double x, double y)
{
    _secondLatestMousePos = _latestMousePos;
    _latestMousePos = glm::vec2(1-x, 1-y);

    if(_rotateCam && _cam != 0 && _useMouseNavigation)
    {
        glm::vec2 diff = glm::normalize(_secondLatestMousePos - _latestMousePos) * 0.025f;

        _cam->setRelativeOrientation(diff.y, diff.x, 0.0f);
    }
}

void ezr::Window::mouseButtonCallback(GLFWwindow *win, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
        _rotateCam = true;
    if(action == GLFW_RELEASE)
        _rotateCam = false;
}

void ezr::Window::enableMouseNavigation(bool val)
{
    _useMouseNavigation = val;
}

void ezr::Window::enableDepth(bool val)
{
    if(val)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void ezr::Window::enableViewports(bool val)
{
_useViewports = val;
}

void ezr::Window::useViewport(GLuint viewport)
{
    if(viewport == 0)
        glViewport(0, 0, _width, _height);
    else if(viewport == 1)
        glViewport(0,0, _width, _height * 0.8f);
    else
        glViewport((_width * 0.2f) * (viewport - 2) ,_height * 0.8f, _width * 0.2f, _height * 0.2f);

}

GLFWwindow* ezr::Window::getWindow() {
    return _window;
}

glm::vec2 ezr::Window::getCursorPosition()
{
    double x, y;
    glfwGetCursorPos(_window, &x, &y );

    return glm::vec2(x, y);
}

glm::ivec2 ezr::Window::getWindowSize()
{
    int width;
    int height;

    glfwGetWindowSize(_window, &width, &height);
    return glm::ivec2(width, height);
}
