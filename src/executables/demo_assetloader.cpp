


#include "Utils/assets.h"
#include "IO/Window.h"
#include "IO/Camera.h"
#include "Renderables/Quad.h"
#include "Renderables/Cube.h"

#include "Shaders/SimpleShader.h"
#include "FBO/FBO.h"

int width = 900;
int height = 600;

int ss = 1;

int main()
{
    //Window
    ezr::Window win("testwindow", width, height);

    //Camera
    ezr::Camera cam;
    win.setCam(&cam);

    //Renderables
    const aiScene * cvark = ezr::assets::import(EZR_PROJECT_PATH "/resources/zwark.dae");
    std::vector<ezr::Renderable *> cvark_renderables = ezr::assets::createRenderables(cvark);
    ezr::Quad quad;
    ezr::Cube cube(5.0f);

    //Shaders
    ezr::SimpleShader gbuffer(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/RimLight/GBuffer.frag");
    ezr::SimpleShader gbuffer_tex(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/GBuffer_tex.frag");
    ezr::SimpleShader simpleTex(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");
    ezr::SimpleShader rimLight(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/RimLight/simpleRimLight.frag");


    //FBO
    ezr::FBO fboGBuffer(ss * width, ss * height);

    //SkyBox Texture
    ezr::Texture texture(EZR_PROJECT_PATH "/resources/space.jpg");

    //FBO Textures
    ezr::Texture positionTex = fboGBuffer.generateColorBuffer("position", 0);
    ezr::Texture uvTex = fboGBuffer.generateColorBuffer("uv", 1);
    ezr::Texture normalTex = fboGBuffer.generateColorBuffer("normal", 2);
    ezr::Texture colorTex = fboGBuffer.generateColorBuffer("color", 3);
    ezr::Texture tangentTex = fboGBuffer.generateColorBuffer("tangent", 4);
    ezr::Texture bitangentTex = fboGBuffer.generateColorBuffer("bitangent", 5);
    ezr::Texture rimlightTex = fboGBuffer.generateColorBuffer("rimlightAmount", 6);
    ezr::Texture depthTex = fboGBuffer.generateDepthBuffer("depth");

    //After textures were created, the FBO can be compiled
    fboGBuffer.compile();

    //FBO
    ezr::FBO fboRimLight(ss * width, ss * height);

    //FBO Textures
    ezr::Texture finalFrameTex = fboRimLight.generateColorBuffer("finalFrame", 0);

    //After textures were created, the FBO can be compiled
    fboRimLight.compile();

    //Model Matrices
    glm::mat4 cvark_model = glm::translate(glm::vec3(0.0f, -0.25f, 0.0f)) * glm::scale(glm::mat4(), glm::vec3(0.25f));
    glm::mat4 cube_model = glm::scale(glm::vec3(-1.0f));
    glm::mat4 perspectiveMat = glm::perspective(glm::radians(60.0f), static_cast<float>(width)/height , 0.10f, 100.0f);


    ezr::debug::println(glm::inverse(perspectiveMat));
    ezr::debug::println(perspectiveMat);


    //Main Loop (if all shaders were compiled correctly)
    if(gbuffer.isOK() && simpleTex.isOK() && gbuffer_tex.isOK())
    {
        while(!win.shouldClose())
            {
                //Bind FBO
                fboGBuffer.bind();
                glViewport(0,0, ss * width,  ss * height);

                //Clear FBO's Buffers
                win.clear();

                //PREPARE CUBE SHADER
                gbuffer_tex.bind();
                gbuffer_tex.uploadUniform(cube_model, "modelMatrix");
                gbuffer_tex.uploadUniform(cam.getRotation(), "viewMatrix");
                gbuffer_tex.uploadUniform(perspectiveMat, "projectionMatrix");
                gbuffer_tex.uploadUniform(texture, "diffuseColor");

                cube.draw();

                //Enable Depth (so Cvark will be rendered on top of the Skybox)
                win.enableDepth(true);

                //PREPARE CVARK SHADER
                gbuffer.bind();
                gbuffer.uploadUniform(cvark_model, "modelMatrix");
                gbuffer.uploadUniform(cam.getViewMatrix(), "viewMatrix");
                gbuffer.uploadUniform(perspectiveMat, "projectionMatrix");
                gbuffer.uploadUniform(0.75f, "rimlightAmount");

                //Render CVark
                for(auto c : cvark_renderables)
                {
                    gbuffer.uploadUniform(c->getAttribute(ezr::Matkey_v3::DIFFUSE_COLOR), "diffuseColor");
                    c->draw();
                }

                //Unuse FBO
                fboGBuffer.unbind();

                win.enableDepth(false);

                rimLight.bind();

                fboRimLight.bind();

                rimLight.uploadUniform(normalTex, "normalTex", 1);
                rimLight.uploadUniform(colorTex, "colorTex", 2);
                rimLight.uploadUniform(rimlightTex, "rimlightTex", 3);
                quad.draw();

                fboRimLight.unbind();

                //Draw mini Viewports
                simpleTex.bind();

                simpleTex.uploadUniform(glm::vec2(width, height), "resolution");

                win.useViewport(2);
                simpleTex.uploadUniform(depthTex, "tex", 0);
                simpleTex.uploadUniform(depthTex, "depthTex", 1);
                quad.draw();

                win.useViewport(3);
                simpleTex.uploadUniform(positionTex, "tex", 0);
                simpleTex.uploadUniform(depthTex, "depthTex", 1);
                quad.draw();

                win.useViewport(4);
                simpleTex.uploadUniform(normalTex, "tex", 0);
                quad.draw();

                win.useViewport(5);
                simpleTex.uploadUniform(colorTex, "tex", 0);
                quad.draw();

                win.useViewport(6);
                simpleTex.uploadUniform(rimlightTex, "tex", 0);
                quad.draw();

                //Compose Frame
                win.useViewport(1);
                simpleTex.uploadUniform(finalFrameTex, "tex", 0);
                simpleTex.uploadUniform(depthTex, "depthTex", 1);
                quad.draw();

                //END OF FRAME
                win.swapBuffer();
            }
    }


    return 0;
}
