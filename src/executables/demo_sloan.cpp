//
// Created by rheidrich on 17.03.17.
//

#include <IO/Window.h>
#include <FBO/Texture.h>
#include <Shaders/SimpleShader.h>
#include <Utils/ShaderUtils.h>
#include <Renderables/Renderable.h>
#include <Utils/assets.h>
#include <Renderables/Quad.h>
#include <Renderables/Cube.h>

int height = 720;
int width = 1280;

int main()
{
    ezr::Window win("Sloan Demo", width, height, 10, 10, false);
    ezr::Camera cam;

    win.setCam(&cam);

    ezr::Texture litsphere(EZR_PROJECT_PATH "/resources/litsphere03.png");

    //ASSETS
    std::vector<ezr::Renderable *> cvarkRenderables = ezr::assets::createRenderables(
            EZR_PROJECT_PATH "/resources/torus.dae");
    std::vector<ezr::Renderable *> sceneRenderables; // = ezr::assets::createRenderables(
            //EZR_PROJECT_PATH "/resources/demo_scene/demo_scene.dae");
    sceneRenderables.insert(sceneRenderables.end(), cvarkRenderables.begin(), cvarkRenderables.end());

    ezr::Quad quad;
    ezr::Cube skybox;

    //SHADERS
    auto sceneGBuffer = ezr::ShaderUtils::createGBuffers(sceneRenderables);
    auto sloanShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                      EZR_SHADER_PATH "/Sloan/sloan.frag");
    auto quadShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                      EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");

    //FBOs

    ezr::FBO fboGBuffer(width, height);
    auto fboTextures = ezr::ShaderUtils::createGBufferFBOTextures(&fboGBuffer);
    fboGBuffer.compile();

    ezr::FBO fboSloan(width, height);
    auto texSloan = fboSloan.generateColorBuffer("output", 0);
    fboSloan.compile();

    //VARIABLES
    glm::mat4 projectionMat = glm::perspective(glm::radians(60.0f), float(width)/height, 0.1f, 100.0f);

    while(!win.shouldClose())
    {
        win.clear();


        //GBUFFER
        fboGBuffer.bind();
        win.useViewport(0);
        win.clear();

        for (int i = 0; i < sceneGBuffer.size(); ++i)
        {
            auto shader = sceneGBuffer[i];
            auto renderable = sceneRenderables[i];
            shader.bind();
            shader.uploadUniform(projectionMat, "projectionMatrix");
            shader.uploadUniform(cam.getViewMatrix(), "viewMatrix");
            ezr::ShaderUtils::uploadGBufferMaterialUniforms(shader, *renderable);

            renderable->draw();
        }

        fboGBuffer.unbind();

        // SLOAN
        fboSloan.bind();
        win.useViewport(0);
        win.clear();
        sloanShader.bind();
        sloanShader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 0);
        sloanShader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 1);
        sloanShader.uploadUniform(litsphere, "litSphereTex", 2);
        sloanShader.uploadUniform(cam.getViewMatrix(), "viewMatrix");
        quad.draw();

        fboSloan.unbind();

        // OUTPUT
        quadShader.bind();
        quadShader.uploadUniform(texSloan, "tex", 0);
        quad.draw();

        win.swapBuffer();
    }




    return 0;
}