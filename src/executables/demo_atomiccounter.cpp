//
// Created by rheidrich on 11.07.17.
//

#include <IO/Window.h>
#include <Shaders/SimpleShader.h>
#include <Utils/ShaderUtils.h>
#include <Renderables/Quad.h>
#include <SSBO/AtomicCounter.h>
#include <Utils/CollectionsUtils.h>
#include <Utils/TextureUtils.h>
#include <Utils/assets.h>
#include <Utils/TextureToViewportRenderer.h>
#include <FBO/TextureArray.h>

int height = 720;
int width = 1280;

bool withColor = false;

int num_samples = 30;
float border_threshold = 5.0f;
float contrast = 1.0f;
float radius = 0.05f;

int main()
{
    //Window and Cam
    ezr::Window win("Atomic Counter Demo", width, height, 10, 10, false);
    ezr::Camera camera;
    camera.setCamera(glm::lookAt(glm::vec3(0, 0, -10.0f), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f)));
    win.setCam(&camera);

    ezr::SimpleShader atomicShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/SSBO_test/atomicShader.frag");
    ezr::SimpleShader imageArrayShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/SSBO_test/imageArrayShader.frag");

    ezr::Quad quad;
    ezr::TextureToViewportRenderer textureToViewportRenderer;

    ezr::AtomicCounter atomicCounter;

    ezr::Texture texA = ezr::TextureUtils::loadFromFile(EZR_PROJECT_PATH "/resources/cubemap/00.png");
    ezr::Texture texB = ezr::TextureUtils::loadFromFile(EZR_PROJECT_PATH "/resources/cubemap/01.png");
    ezr::Texture texC = ezr::TextureUtils::loadFromFile(EZR_PROJECT_PATH "/resources/cubemap/02.png");


    std::vector<ezr::Renderable *> cvark_renderables = ezr::assets::createRenderables(
            EZR_PROJECT_PATH "/resources/cvark_singlemesh.dae");
    ezr::Renderable cvark = *cvark_renderables[0];
    ezr::SimpleShader gbufferShader = ezr::ShaderUtils::createGBuffer(cvark);

    int sizeFBO = 1024;
    ezr::FBO gbufferFBO(sizeFBO, sizeFBO, GL_RGBA8);
    auto gbufferTextures = ezr::ShaderUtils::createGBufferFBOTextures(&gbufferFBO);
    gbufferFBO.compile();

//    glm::mat4 perspective = glm::ortho(-10, 10, -10, 10, 0, 100);
    glm::mat4 perspective = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

    ezr::TextureArray textureArray(1024, 1024, 3);
    ezr::TextureArray textureArray2(512, 512, 3);


    while(!win.shouldClose())
    {
        atomicCounter.reset(0);

        gbufferFBO.bind();
        glViewport(0,0, sizeFBO, sizeFBO);
        win.clear(glm::vec3(0,1,0.5));
        gbufferShader.bind();

        gbufferShader.uploadUniform(perspective, "projectionMatrix");
        gbufferShader.uploadUniform(camera.getViewMatrix(), "viewMatrix");
        ezr::ShaderUtils::uploadGBufferMaterialUniforms(gbufferShader, cvark);
        cvark.draw();

        gbufferFBO.unbind();

        win.clear();
        win.useViewport(1);
        atomicShader.bind();
        atomicCounter.bind(0);
        atomicShader.uploadUniform((float) height * width, "numPixels");

        quad.draw();


        textureArray.fillWithTextures(std::vector<ezr::Texture>({gbufferTextures[ezr::NORMALS_OUTPUT], texB, texC}), 1);
        textureArray2.fillWithTextures(std::vector<ezr::Texture>({gbufferTextures[ezr::NORMALS_OUTPUT], texB, texC}), 1);


        int indexWeh = ((int) glfwGetTime()) % 3;

        win.useViewport(2);
        imageArrayShader.bind();
        imageArrayShader.uploadUniform(indexWeh, "counter");
        imageArrayShader.uploadUniform(textureArray, "texAvery", 0);
        textureArray.bindImageTexture(0);
        quad.draw();

        win.useViewport(3);
        textureToViewportRenderer.renderTexture(gbufferTextures[ezr::NORMALS_OUTPUT]);
        win.useViewport(4);
        textureToViewportRenderer.renderTexture(texA);
        win.useViewport(5);
        textureToViewportRenderer.renderTexture(textureArray, indexWeh);
        win.useViewport(6);
        textureToViewportRenderer.renderTexture(textureArray2, indexWeh);

        win.swapBuffer();
    }

}