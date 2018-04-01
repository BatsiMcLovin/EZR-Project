#include <Shaders/SimpleShader.h>
#include <Renderables/Quad.h>
#include "IO/Window.h"
#include "Utils/assets.h"
#include "Renderables/Cube.h"
#include "FBO/FBO.h"
#include "AntiAliasing/MLAA.h"


int width = 900;
int height = 600;

int main()
{

    //Init Window
    ezr::Window win("Multisampling Demo", width, height, 50, 50);

    //Camera
    ezr::Camera camera;
    win.setCam(&camera);

    camera.setPosition(glm::vec3(0.0f, 3.0f, 4.0f));

    //Assets
    const aiScene * cvark_mesh = ezr::assets::import(EZR_PROJECT_PATH "/resources/zwark.dae");
    std::vector<ezr::Renderable*> cvark_renderables = ezr::assets::createRenderables(cvark_mesh);
    ezr::Cube skybox;
    ezr::Quad quad;

    //Shaders
    ezr::SimpleShader gbuffer(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/GBuffer.frag");
    ezr::SimpleShader quadDraw(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");

    //AntiAliasing
    ezr::MLAA mlaa(width, height);

    //FBO for GBuffer
    ezr::FBO fboGBuffer(width, height);

    //FBO Textures
    ezr::Texture posTex = fboGBuffer.generateColorBuffer("positionOutput", 0);
    ezr::Texture uvTex = fboGBuffer.generateColorBuffer("uvOutput", 1);
    ezr::Texture nrmTex = fboGBuffer.generateColorBuffer("normalOutput", 2);
    ezr::Texture colorTex = fboGBuffer.generateColorBuffer("colorOutput", 3);
    ezr::Texture depthTex = fboGBuffer.generateDepthBuffer("depthOutput");

    fboGBuffer.compile();

    while(!win.shouldClose())
    {
        //clear all buffers
        fboGBuffer.bind();
        win.clear();

        //use GBuffer Shader
        gbuffer.bind();

        //draw Skybox

        gbuffer.uploadUniform(glm::scale(glm::vec3(-10.0f)), "modelMatrix");
        gbuffer.uploadUniform(camera.getRotation(), "viewMatrix");
        gbuffer.uploadUniform(glm::perspective(glm::radians(60.0f), 1.0f, 0.10f, 100.00f), "projectionMatrix");
        gbuffer.uploadUniform(glm::vec3(0.5, 0.1, 0.0), "diffuseColor");

        skybox.draw();

        //enable depth
        win.enableDepth(true);

        //draw cvark

        gbuffer.uploadUniform(glm::mat4(), "modelMatrix");
        gbuffer.uploadUniform(camera.getViewMatrix(), "viewMatrix");
        gbuffer.uploadUniform(glm::perspective(glm::radians(60.0f), 1.0f, 0.10f, 100.00f), "projectionMatrix");

        for (auto c : cvark_renderables)
        {
            gbuffer.uploadUniform(c->getAttribute(ezr::Matkey_v3::DIFFUSE_COLOR), "diffuseColor");
            c->draw();
        }

        fboGBuffer.unbind();
        win.enableDepth(false);

        //Anti Aliasing
        ezr::Texture* filteredTex = mlaa.filter(&colorTex);

        //Draw mini Viewports
        quadDraw.bind();
        win.useViewport(0);
        quadDraw.uploadUniform(filteredTex, "tex", 0);
        quad.draw();

        //Swap buffers
        win.swapBuffer();
    }

    return 0;
}
