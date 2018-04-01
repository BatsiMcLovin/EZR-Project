//
// Created by dino on 22.12.15.
//

#include <Shaders/SimpleShader.h>
#include <Renderables/Quad.h>
#include <Utils/ShaderUtils.h>
#include "IO/Window.h"
#include "Utils/assets.h"
#include "Renderables/Cube.h"
#include "FBO/FBO.h"
#include "AntiAliasing/MLAA.h"


float thick = 0.01f;
float threshholdDepth = 0.008;
float threshholdDiffuse = 0.01;
float threshholdNormal = 0.85;

int height = 720;
int width = 1280;

int shadow_width = 2000;
int shadow_height = 2000;

glm::vec3 pointLightPos = glm::vec3(-10.687358f, 11.614994f, 10.442025f);

float shadow_border_threshold = 0.5f;

bool oldSchoolGraphics = false;

int main()
{

    //Init Window
    ezr::Window win("Toon Demo", width, height, 50, 50, false);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(win.getWindow(), false);

    //Camera
    ezr::Camera camera00;
    camera00.setCamera(glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f)));
    win.setCam(&camera00);

    ezr::Camera camera01;
    camera01.setCamera(glm::lookAt(pointLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    //Assets
    std::vector<ezr::Renderable*> cvark_renderables = ezr::assets::createRenderables(EZR_PROJECT_PATH "/resources/zwark.dae");
    std::vector<ezr::Renderable*> scene_renderables = ezr::assets::createRenderables(EZR_PROJECT_PATH "/resources/demo_scene/demo_scene.dae");

    ezr::Cube skybox;
    ezr::Quad quad;

    //AntiAliasing
    ezr::MLAA mlaa(width, height);

    //Textures
    ezr::Texture toonRamp(EZR_PROJECT_PATH "/resources/toonRamp.png");

    //Shaders
    ezr::SimpleShader gbuffer(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/GBuffer.frag");
    ezr::SimpleShader gbuffer_tex(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/GBuffer_tex.frag");
    ezr::SimpleShader toonComp(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/Toon/toon.frag");
    ezr::SimpleShader simpleTex(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");
    ezr::SimpleShader outLine(EZR_SHADER_PATH "/Toon/outLine.vert", EZR_SHADER_PATH "/Toon/outLine.frag");
    ezr::SimpleShader ssrShader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/SSR/ssr.frag");
    ezr::SimpleShader ssrResultShader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/SSR/ssrDraw.frag");
    ezr::SimpleShader depth_shader(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/ShadowMapping/shadowDraw.frag" );
    ezr::SimpleShader shadowmap_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/ShadowMapping/pcfShadowMap.frag" );
    ezr::SimpleShader gameboy_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                                       EZR_SHADER_PATH "/PostProcessing/GameBoy/gameboy.frag");

    //FBO for GBuffer
    ezr::FBO fbo_gbuffer(width, height);
    auto fboTextures = ezr::ShaderUtils::createGBufferFBOTextures(&fbo_gbuffer);
    fbo_gbuffer.compile();

    //FBO for outLines and Sobel edges
    ezr::FBO fboEdge(width, height);

    ezr::Texture outLineTex = fboEdge.generateColorBuffer("outLineOutput", 0);

    fboEdge.compile();
    /**/
    //FBO for Toon
    ezr::FBO fboToon(width, height);

    ezr::Texture sobelTex = fboToon.generateColorBuffer("sobelOutput", 0);
    ezr::Texture cellTex = fboToon.generateColorBuffer("cellOutput", 1);
    ezr::Texture compTex = fboToon.generateColorBuffer("compositionOutput", 2);

    fboToon.compile();

    //FBO for SSR
    ezr::FBO fboSSR(width, height);
    ezr::Texture reflectionsTex = fboSSR.generateColorBuffer("reflectionsOutput", 0);
    fboSSR.compile();

    //FBO for SSR Result -.-
    ezr::FBO fboSSRResult(width, height);
    ezr::Texture ssrResult = fboSSRResult.generateColorBuffer("reflectionsMapped", 0);
    fboSSRResult.compile();

    //FBO POST PROCESSING================================

    float gameboy_scalor = 0.2f;
    ezr::FBO fbo_postprocessing(width * gameboy_scalor, height * gameboy_scalor);

    ezr::Texture postprocessing_tex = fbo_postprocessing.generateColorBuffer("pp", 0);

    fbo_postprocessing.compile();

    postprocessing_tex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    //FBO DEPTH DRAW================================

    ezr::FBO fbo_depth(shadow_width, shadow_height);

    ezr::Texture position_depth_tex = fbo_depth.generateColorBuffer("position", 0);
    ezr::Texture depth_depth_tex = fbo_depth.generateDepthBuffer("depth");

    fbo_depth.compile();

    //FBO SHADOW MAP================================

    ezr::FBO fbo_shadowmap(width, height);

    ezr::Texture shadowmap_tex = fbo_shadowmap.generateColorBuffer("shadowmap", 0);

    fbo_shadowmap.compile();

    //DEPTH TEXTURE PASS================================

    glm::mat4 depth_perspective = glm::perspective(glm::radians(30.0f), 1.0f, 4.5f, 50.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    fbo_depth.bind();
    win.clear();
    glViewport(0,0, shadow_width, shadow_height);

    depth_shader.bind();

    depth_shader.uploadUniform(glm::mat4(), "modelMatrix");
    depth_shader.uploadUniform(camera01.getViewMatrix(), "viewMatrix");
    depth_shader.uploadUniform(depth_perspective, "projectionMatrix");


    glm::vec4 clear_color = glm::vec4(114, 144, 154, 0);
    glm::mat4 perspective = glm::perspective(glm::radians(60.0f), 1.0f, 0.10f, 100.00f);

    for(auto c : cvark_renderables)
    {
        c->draw();
    }

    depth_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

    for(auto c : scene_renderables)
    {
        c->draw();
    }


    fbo_depth.unbind();
    glDisable(GL_CULL_FACE);

    int gameboy_step_size = 8;
    bool gameboy_use_green = false;

    while(!win.shouldClose())
    {


        //clear all buffers
        fbo_gbuffer.bind();
        win.clear();

        //use GBuffer Shader
        gbuffer.bind();

        //draw Skybox

        gbuffer.uploadUniform(glm::scale(glm::vec3(10.0f)), "modelMatrix");
        gbuffer.uploadUniform(camera00.getRotation(), "viewMatrix");
        gbuffer.uploadUniform(perspective, "projectionMatrix");
        gbuffer.uploadUniform(glm::vec3(0.0, 0.1, 0.5), "diffuseColor");

        skybox.draw();

        //draw outLines
        fboEdge.bind();
        win.clear(glm::vec3(1));
        win.enableDepth(false);
        outLine.bind();
        outLine.uploadUniform(glm::mat4(), "modelMatrix");
        outLine.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        outLine.uploadUniform(perspective, "projectionMatrix");
        outLine.uploadUniform(thick, "thickness");
        outLine.uploadUniform(glm::vec4(0, 0, 0, 1), "sillouetteColor");
        for (auto c : cvark_renderables)
        {
            c->draw();
        }
        outLine.uploadUniform(0.0f, "thickness");
        outLine.uploadUniform(glm::vec4(1, 1, 1, 1), "sillouetteColor");
        for (auto c : cvark_renderables)
        {
            c->draw();
        }
        fboEdge.unbind();

        fbo_gbuffer.bind();
        win.enableDepth(true);
        gbuffer.bind();

        //draw cvark

        gbuffer.uploadUniform(glm::mat4(), "modelMatrix");
        gbuffer.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        gbuffer.uploadUniform(perspective, "projectionMatrix");

        int matID = 0;

        for (auto c : cvark_renderables)
        {
            matID++;

            gbuffer.uploadUniform(matID, "materialID");
            gbuffer.uploadUniform(c->getAttribute(ezr::Matkey_v3::DIFFUSE_COLOR), "diffuseColor");
            c->draw();
        }

        gbuffer.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

        gbuffer_tex.bind();

        gbuffer_tex.uploadUniform(glm::mat4(), "modelMatrix");
        gbuffer_tex.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        gbuffer_tex.uploadUniform(perspective, "projectionMatrix");

        gbuffer_tex.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");
        for (auto c : scene_renderables)
        {
            matID++;

            if(c->hasAttribute(ezr::Matkey_tex2D::DIFFUSE_TEXTURE))
            {
                gbuffer_tex.bind();
                gbuffer_tex.uploadUniform(c->getAttribute(ezr::Matkey_tex2D::DIFFUSE_TEXTURE), "diffuseColor", 0);
                gbuffer_tex.uploadUniform(matID, "materialID");

                if(c->hasAttribute(ezr::Matkey_v1::SHININESS))
                    gbuffer_tex.uploadUniform(c->getAttribute(ezr::Matkey_v1::SHININESS), "reflectivity");
                else
                    gbuffer_tex.uploadUniform(0.0f, "reflectivity");
            } else
            {
                gbuffer.bind();
                gbuffer.uploadUniform(c->getAttribute(ezr::Matkey_v3::DIFFUSE_COLOR), "diffuseColor");
                gbuffer.uploadUniform(matID, "materialID");

                if(c->hasAttribute(ezr::Matkey_v1::REFLECTIVITY))
                    gbuffer.uploadUniform(c->getAttribute(ezr::Matkey_v1::REFLECTIVITY), "reflectivity");
                else
                    gbuffer.uploadUniform(0.0f, "reflectivity");

            }


            c->draw();
        }

        fbo_gbuffer.unbind();

        //SHADOW MAPPING================================

        fbo_shadowmap.bind();
        win.clear();

        shadowmap_shader.bind();
        shadowmap_shader.uploadUniform(camera00.getViewMatrix(),"viewMatrix");
        shadowmap_shader.uploadUniform(perspective,"projectionMatrix");
        shadowmap_shader.uploadUniform(camera01.getViewMatrix(),"shadowViewMatrix");
        shadowmap_shader.uploadUniform(depth_perspective,"shadowProjMatrix");
        shadowmap_shader.uploadUniform(pointLightPos, "lightPosition");
        shadowmap_shader.uploadUniform(&depth_depth_tex, "shadowTex", 0);
        shadowmap_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
        shadowmap_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 2);

        quad.draw();

        fbo_shadowmap.unbind();


        //Draw mini Viewports
        win.enableDepth(false);
        simpleTex.bind();

        win.useViewport(2);
        simpleTex.uploadUniform(fboTextures[ezr::SPECULAR_OUTPUT], "tex", 0);
        quad.draw();

        win.useViewport(3);
        simpleTex.uploadUniform(&position_depth_tex, "tex", 0);
        quad.draw();

        win.useViewport(4);
        simpleTex.uploadUniform(&shadowmap_tex, "tex", 0);
        quad.draw();

        win.useViewport(0);
        fboSSR.bind();
        ssrShader.bind();

        ssrShader.uploadUniform(fboTextures[ezr::DEPTH_OUTPUT], "depthTex", 0);
        ssrShader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 1);
        ssrShader.uploadUniform(fboTextures[ezr::DIFFUSE_OUTPUT], "colorTex", 2);
        ssrShader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 3);
        ssrShader.uploadUniform(fboTextures[ezr::SPECULAR_OUTPUT], "reflectivityTex", 4);
        ssrShader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        ssrShader.uploadUniform(glm::perspective(glm::radians(60.0f), 1.0f, 0.10f, 100.00f), "projectionMatrix");
        ssrShader.uploadUniform(width, "width");
        ssrShader.uploadUniform(height, "height");
        ssrShader.uploadUniform(0.10f, "near");
        ssrShader.uploadUniform(100.00f, "far");
        quad.draw();

        fboSSR.unbind();

        //Compose Frame

//        win.useViewport(0);
        fboToon.bind();

        toonComp.bind();

        toonComp.uploadUniform(glm::vec3(-10.687358f, 9.614994f, 10.442025f), "pointLight");


        toonComp.uploadUniform(perspective, "projectionMatrix");

        toonComp.uploadUniform(shadow_border_threshold, "shadow_border_threshold");
        toonComp.uploadUniform(fboTextures[ezr::DEPTH_OUTPUT], "depthTex", 0);
        toonComp.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 1);
        toonComp.uploadUniform(fboTextures[ezr::DIFFUSE_OUTPUT], "colorTex", 2);
        toonComp.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 3);
        toonComp.uploadUniform(toonRamp, "toonRamp", 4);
        toonComp.uploadUniform(outLineTex, "outLineTex", 5);
        toonComp.uploadUniform(fboTextures[ezr::MATERIAL_ID_OUTPUT], "materialTex", 6);
        toonComp.uploadUniform(shadowmap_tex, "shadowTex", 7);
        toonComp.uploadUniform(threshholdDepth, "threshholdDepth");
        toonComp.uploadUniform(threshholdDiffuse, "threshholdDiffuse");
        toonComp.uploadUniform(threshholdNormal, "threshholdNormal");

        quad.draw();


        fboToon.unbind();

        fboSSRResult.bind();
        ssrResultShader.bind();
        ssrResultShader.uploadUniform(compTex, "texA", 0);
        ssrResultShader.uploadUniform(reflectionsTex, "texB", 1);

        quad.draw();
        fboSSRResult.unbind();

        //GAME BOY================================
        if (oldSchoolGraphics)
        {
            fbo_postprocessing.bind();
            win.clear();
            glViewport(0, 0, width * gameboy_scalor, height * gameboy_scalor);

            gameboy_shader.bind();
            gameboy_shader.uploadUniform(compTex, "tex");
            gameboy_shader.uploadUniform(gameboy_use_green, "gameboy_use_green");
            gameboy_shader.uploadUniform(gameboy_step_size, "steps");
            quad.draw();

            fbo_postprocessing.unbind();
        }


        //mini Viewports again
        simpleTex.bind();

        simpleTex.uploadUniform(glm::vec2(width, height), "resolution");


        win.useViewport(5);
        simpleTex.uploadUniform(reflectionsTex, "tex", 0);
        quad.draw();

        win.useViewport(6);
        simpleTex.uploadUniform(cellTex, "tex", 0);
        quad.draw();

        if(oldSchoolGraphics)
        {
            win.useViewport(1);
            simpleTex.uploadUniform(postprocessing_tex, "tex", 0);
            quad.draw();
        }
        else
        {
            win.useViewport(1);
            simpleTex.uploadUniform(compTex, "tex", 0);
            quad.draw();
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGui::SliderFloat("shadow_border_threshold", &shadow_border_threshold, 0.0f, 1.0f);
            ImGui::SliderFloat("Outline-Thickness", &thick, 0.01f, 0.3f);
            ImGui::SliderFloat("Sobel on Depth", &threshholdDepth, 0.0f, 0.009f);
            ImGui::SliderFloat("Sobel on Normals", &threshholdNormal, 0.5f, 1.0f);
            ImGui::SliderFloat("Sobel on diffuse color", &threshholdDiffuse, 0.005f, 0.02f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Checkbox("Use oldschool graphics", &oldSchoolGraphics);
            if (oldSchoolGraphics)
            {
                ImGui::Checkbox("Use GAMEBOY green", &gameboy_use_green);
                ImGui::SliderInt("Steps", &gameboy_step_size, 1, 16);
            }
        }
        ImGui::Render();

        bool enableMouse = !io.WantCaptureMouse;

        win.enableMouseNavigation(enableMouse);
        //Swap buffers
        win.swapBuffer();


    }

    return 0;
}
