#include <IO/Window.h>
#include <Renderables/Renderable.h>
#include <Utils/assets.h>
#include <Renderables/Quad.h>
#include <Renderables/Cube.h>
#include <Shaders/SimpleShader.h>
#include <FBO/FBO.h>
#include <SSBO/SSBO.h>

#include <glm/gtx/color_space.hpp>
#include <random>

int height = 720;
int width = 1280;

std::vector<glm::vec4> fillData(int size)
{
    std::vector<glm::vec4> container;
    container.resize(size);



    for(int i = 0; i < size; i++)
    {
        float randomNo = std::rand() % 360;

        glm::vec3 color = glm::hsvColor(glm::vec3(randomNo, 1.0f, 1.0f));

        color = glm::vec3((std::rand() % 100) + 1, std::rand() % 100, std::rand() % 100);
        color = glm::normalize(color);

        container[i] = glm::vec4(color.x, color.y, color.z, 1.0f);
    }
    return container;
}

int main()
{
    // //////////////////////////////////////
    // INITIALIZATION

    ezr::Window win("SSBO", width, height, 100, 100, false);
    ezr::Camera camera00;
    camera00.setCamera(glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f)));
    win.setCam(&camera00);

    //ASSETS
    auto scene = ezr::assets::import(EZR_PROJECT_PATH "/resources/zwark.dae");
    scene = ezr::assets::removeDoublesAndSmooth(scene);
    std::vector<ezr::Renderable*> cvarkRenderables = ezr::assets::createRenderables(scene);

    //SHADERS
    ezr::SimpleShader forward_shader(EZR_SHADER_PATH "/medcv/simpleTransform.vert", EZR_SHADER_PATH "/medcv/simpleTexture.frag");
    ezr::SimpleShader ssbo_shader(EZR_SHADER_PATH "/SSBO_test/simpleTransform_ssboRead.vert", EZR_SHADER_PATH "/SSBO_test/simpleTexture_ssboWrite.frag");

    //TEXTURES
    ezr::Texture tex(EZR_PROJECT_PATH "/resources/best.jpg");

    //MISC
    ezr::Quad quad;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(40.0f), static_cast<float>(width)/height, 0.1f, 100.0f);

    //FILL SSBO
    std::vector<glm::vec4> colorSSBOdata = fillData(cvarkRenderables[1]->getVertexCount());
    ezr::SSBO ssbo(colorSSBOdata);

    ezr::debug::println("MAIN LOOP WILL RUN NOW");

    // //////////////////////////////////////
    // MAIN LOOP
    while(!win.shouldClose())
    {
        glm::mat4 viewMatrix = camera00.getViewMatrix();
        win.clear();


        //ALL OF CVARK
        forward_shader.bind();

        forward_shader.uploadUniform(viewMatrix, "viewMatrix");
        forward_shader.uploadUniform(projectionMatrix, "projectionMatrix");
        forward_shader.uploadUniform(tex, "tex");
        for(ezr::Renderable* renderable : cvarkRenderables)
        {       if(renderable != cvarkRenderables[1])
            {
                renderable->draw();
            }
        }

        // ONLY THE SSBO PART

        ssbo_shader.bind();

        ssbo.bind();

        ssbo_shader.uploadUniform(viewMatrix, "viewMatrix");
        ssbo_shader.uploadUniform(projectionMatrix, "projectionMatrix");
        ssbo_shader.uploadUniform(tex, "tex");

        cvarkRenderables[1]->draw();


        win.swapBuffer();
    }

    return 0;
}
