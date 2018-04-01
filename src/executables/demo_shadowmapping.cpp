//
// Created by dino on 11.01.16.
//

#include <FBO/FBO.h>
#include <Renderables/Quad.h>
#include <Renderables/Cube.h>
#include <Utils/assets.h>
#include <AntiAliasing/MLAA.h>
#include <Utils/ShaderUtils.h>
#include "IO/Window.h"
#include "Shaders/SimpleShader.h"


int height = 720;
int width = 1280;

int shadow_width = 2000;
int shadow_height = 2000;

int main()
{

    //Create Window
    ezr::Window win("Shadow Mapping Demo", width, height, 10, 10, false);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(win.getWindow(), false);

    //Create Cameras
    ezr::Camera cam00;
    ezr::Camera cam01;

    cam01.setPosition(glm::vec3(4.0f));
    cam01.setOrientation(M_PI_2/4.0f, -M_PI_2/2.0f, 0.0f);

    //Set first cam as navigational camera
    win.setCam(&cam00);

    //MLAA

    ezr::MLAA mlaa(shadow_width, shadow_height);

    //Assets
    ezr::Cube cube;
    ezr::Quad quad;
    auto cvark_mesh = ezr::assets::import(EZR_PROJECT_PATH "/resources/zwark.dae");
    auto cvark_renderables = ezr::assets::createRenderables(cvark_mesh);

    //Shaders
    auto cvark_gbuffers = ezr::ShaderUtils::createGBuffers(cvark_renderables);
    auto cube_gbuffer = ezr::ShaderUtils::createGBuffer(cube);

    ezr::SimpleShader depthdraw_shader(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/ShadowMapping/shadowDraw.frag");
    ezr::SimpleShader simple_shadowmap_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/ShadowMapping/simpleShadowMap.frag");
    ezr::SimpleShader poisson_shadowmap_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/ShadowMapping/poissonShadowMap.frag");
    ezr::SimpleShader both_shadowmap_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/ShadowMapping/pcfAndPoissonShadowMap.frag");
    ezr::SimpleShader pcf_shadowmap_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/ShadowMapping/pcfShadowMap.frag");
    ezr::SimpleShader pcf_shadowmap_shader_advanced(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/ShadowMapping/pcfShadowMap_advanced.frag");
    ezr::SimpleShader quad_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");
    ezr::SimpleShader multiply_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/Misc/multiply.frag");

    //FBO - Shadow Draw Pass

    ezr::FBO fbo_depthdraw(shadow_width, shadow_height);

    ezr::Texture shadowPositionTex = fbo_depthdraw.generateColorBuffer("position", 0);
    ezr::Texture shadowDepthTex = fbo_depthdraw.generateDepthBuffer("depth");

    fbo_depthdraw.compile();

    shadowDepthTex.bind();
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //FBO - Shadow Map Pass

    ezr::FBO mapFBO(width, height);

    ezr::Texture lightnessTex = mapFBO.generateColorBuffer("position", 0);

    mapFBO.compile();

    //FBO - GBUFFER

    ezr::FBO fbo_gbuffer(width,height);

    auto fboTextures = ezr::ShaderUtils::createGBufferFBOTextures(&fbo_gbuffer);

    fbo_gbuffer.compile();

    //FBO - MULTIPLY

    ezr::FBO fbo_multiply(width, height);

    ezr::Texture final_tex = fbo_multiply.generateColorBuffer("final", 0);

    fbo_multiply.compile();


    //precomputed matrices

    glm::mat4 cube_mat01 = glm::translate(glm::vec3(0.0f, 0.75f, 0.0f)) * glm::scale(glm::vec3(0.25f));
    glm::mat4 cube_mat02 = glm::scale(glm::vec3(10.0f, 0.1f, 10.0f));
    glm::mat4 cvark_mat = glm::translate(glm::vec3(-1.0f, 1.0f, 1.0f)) * glm::scale(glm::vec3(0.25f));

    glm::mat4 project_mat = glm::perspective(glm::radians(60.0f), static_cast<float>(width)/height, 0.10f, 100.00f);
    glm::mat4 shadow_project_mat = glm::perspective(glm::radians(30.0f), 1.0f, 1.0f, 20.0f);


    cam01.setCamera( glm::lookAt(cam00.getPosition() + glm::vec3(3.5), cam00.getPosition(), glm::vec3(0.0, 1.0, 0.0)));

    float t = 0.0f;
    int mode = 0;
    int pcfMaxRadius = 10;
    int numPoissonSamples = 20;

    while(!win.shouldClose())
    {
        t += 0.001f;
        cvark_mat = glm::translate(glm::vec3(-1.0f, 1.0f, 1.0f)) * glm::scale(glm::vec3(0.25f)) * glm::rotate(t, glm::vec3(0.0, 1.0, 0.0));

        //Enable Depth test and Culling for Shadowmapping

        glEnable(GL_CULL_FACE);

        win.enableDepth(true);

        fbo_depthdraw.bind();
        win.clear(glm::vec3(0.25f));


        //==== FIRST PASS ====

        depthdraw_shader.bind();
        glCullFace(GL_FRONT);
        glViewport(0, 0, shadow_width, shadow_height);

        depthdraw_shader.uploadUniform(cam01.getViewMatrix(), "viewMatrix");
        depthdraw_shader.uploadUniform(shadow_project_mat, "projectionMatrix");

        depthdraw_shader.uploadUniform(cube_mat01, "modelMatrix");
        cube.draw();

        depthdraw_shader.uploadUniform(cube_mat02, "modelMatrix");
        cube.draw();


        for (auto c : cvark_renderables)
        {
            depthdraw_shader.uploadUniform(cvark_mat, "modelMatrix");
            c->draw();
        }

        fbo_depthdraw.unbind();

        //==== SECOND PASS ====

        glDisable(GL_CULL_FACE);


        fbo_gbuffer.bind();
        win.clear(glm::vec3(0.5f, 0.1f, 0.25f));
        win.useViewport(0);

        cube_gbuffer.bind();

        cube_gbuffer.uploadUniform(cam00.getViewMatrix(), "viewMatrix");
        cube_gbuffer.uploadUniform(project_mat, "projectionMatrix");

        cube_gbuffer.uploadUniform(cube_mat01, "modelMatrix");
        cube_gbuffer.uploadUniform(glm::vec3(0.0f, 0.75f, 0.1f),"diffuseColor");
        cube.draw();

        cube_gbuffer.uploadUniform(cube_mat02, "modelMatrix");
        cube_gbuffer.uploadUniform(glm::vec3(0.5f, 0.0f, 0.0f),"diffuseColor");
        cube.draw();

        for (int i = 0; i < cvark_renderables.size(); i++)
        {
            cvark_gbuffers[i].bind();
            cvark_gbuffers[i].uploadUniform(cam00.getViewMatrix(), "viewMatrix");
            cvark_gbuffers[i].uploadUniform(project_mat, "projectionMatrix");
            cvark_gbuffers[i].uploadUniform(cvark_mat, "modelMatrix");

            ezr::ShaderUtils::uploadGBufferMaterialUniforms(cvark_gbuffers[i], *cvark_renderables[i]);

            cvark_renderables[i]->draw();
        }


        fbo_gbuffer.unbind();

        //THIRD PASS
        std::string mode_str = "";

        if(mode == 0)
        {
            mode_str = "Unfiltered";
            mapFBO.bind();
            win.useViewport(0);
            win.clear(glm::vec3(0.25f));

            simple_shadowmap_shader.bind();

            simple_shadowmap_shader.uploadUniform(&shadowDepthTex, "shadowTex", 0);
            simple_shadowmap_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
            simple_shadowmap_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 2);
            simple_shadowmap_shader.uploadUniform(cam01.getPosition(), "lightPosition");

            simple_shadowmap_shader.uploadUniform(cam00.getViewMatrix(), "viewMatrix");
            simple_shadowmap_shader.uploadUniform(project_mat, "projectionMatrix");
            simple_shadowmap_shader.uploadUniform(shadow_project_mat, "shadowProjMatrix");
            simple_shadowmap_shader.uploadUniform(cam01.getViewMatrix(), "shadowViewMatrix");

            quad.draw();

            mapFBO.unbind();
        }if(mode == 1)
        {
            mode_str = "Poisson Sampling";
            mapFBO.bind();
            win.useViewport(0);
            win.clear(glm::vec3(0.25f));

            poisson_shadowmap_shader.bind();

            poisson_shadowmap_shader.uploadUniform(&shadowDepthTex, "shadowTex", 0);
            poisson_shadowmap_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
            poisson_shadowmap_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 2);
            poisson_shadowmap_shader.uploadUniform(cam01.getPosition(), "lightPosition");

            poisson_shadowmap_shader.uploadUniform(numPoissonSamples, "numPoissonSamples");
            poisson_shadowmap_shader.uploadUniform(cam00.getViewMatrix(), "viewMatrix");
            poisson_shadowmap_shader.uploadUniform(project_mat, "projectionMatrix");
            poisson_shadowmap_shader.uploadUniform(shadow_project_mat, "shadowProjMatrix");
            poisson_shadowmap_shader.uploadUniform(cam01.getViewMatrix(), "shadowViewMatrix");

            quad.draw();

            mapFBO.unbind();
        }if(mode == 2)
        {

            mode_str = "PCF Filtering";
            mapFBO.bind();
            win.useViewport(0);
            win.clear(glm::vec3(0.25f));

            pcf_shadowmap_shader.bind();

            pcf_shadowmap_shader.uploadUniform(&shadowDepthTex, "shadowTex", 0);
            pcf_shadowmap_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
            pcf_shadowmap_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 2);
            pcf_shadowmap_shader.uploadUniform(cam01.getPosition(), "lightPosition");

            pcf_shadowmap_shader.uploadUniform(pcfMaxRadius, "pcfMaxRadius");
            pcf_shadowmap_shader.uploadUniform(cam00.getViewMatrix(), "viewMatrix");
            pcf_shadowmap_shader.uploadUniform(project_mat, "projectionMatrix");
            pcf_shadowmap_shader.uploadUniform(shadow_project_mat, "shadowProjMatrix");
            pcf_shadowmap_shader.uploadUniform(cam01.getViewMatrix(), "shadowViewMatrix");

            quad.draw();

            mapFBO.unbind();
        }if(mode == 3)
        {

            mode_str = "Poisson + PCF";
            mapFBO.bind();
            win.useViewport(0);
            win.clear(glm::vec3(0.25f));

            both_shadowmap_shader.bind();

            both_shadowmap_shader.uploadUniform(&shadowDepthTex, "shadowTex", 0);
            both_shadowmap_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
            both_shadowmap_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 2);
            both_shadowmap_shader.uploadUniform(cam01.getPosition(), "lightPosition");

            both_shadowmap_shader.uploadUniform(numPoissonSamples, "numPoissonSamples");
            both_shadowmap_shader.uploadUniform(cam00.getViewMatrix(), "viewMatrix");
            both_shadowmap_shader.uploadUniform(project_mat, "projectionMatrix");
            both_shadowmap_shader.uploadUniform(shadow_project_mat, "shadowProjMatrix");
            both_shadowmap_shader.uploadUniform(cam01.getViewMatrix(), "shadowViewMatrix");

            quad.draw();

            mapFBO.unbind();
        }if(mode == 4)
        {
            mode_str = "PCF Filtering Advanced";
            mapFBO.bind();
            win.useViewport(0);
            win.clear(glm::vec3(0.25f));

            pcf_shadowmap_shader_advanced.bind();

            pcf_shadowmap_shader_advanced.uploadUniform(&shadowDepthTex, "shadowTex", 0);
            pcf_shadowmap_shader_advanced.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
            pcf_shadowmap_shader_advanced.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 2);
            pcf_shadowmap_shader_advanced.uploadUniform(cam01.getPosition(), "lightPosition");

            pcf_shadowmap_shader_advanced.uploadUniform(pcfMaxRadius, "pcfMaxRadius");
            pcf_shadowmap_shader_advanced.uploadUniform(cam00.getViewMatrix(), "viewMatrix");
            pcf_shadowmap_shader_advanced.uploadUniform(project_mat, "projectionMatrix");
            pcf_shadowmap_shader_advanced.uploadUniform(shadow_project_mat, "shadowProjMatrix");
            pcf_shadowmap_shader_advanced.uploadUniform(cam01.getViewMatrix(), "shadowViewMatrix");

            quad.draw();

            mapFBO.unbind();
        }

        //FINAL COMP PASS

        fbo_multiply.bind();
        win.clear();

        multiply_shader.bind();
        multiply_shader.uploadUniform(fboTextures[ezr::DIFFUSE_OUTPUT], "texA", 0);
        multiply_shader.uploadUniform(&lightnessTex, "texB", 1);

        quad.draw();

        fbo_multiply.unbind();

        //==== QUAD PASS ====
        win.enableDepth(false);

        quad_shader.bind();


        win.useViewport(1);
        quad_shader.uploadUniform(&final_tex, "tex", 0);
        quad.draw();
        win.useViewport(2);
        quad_shader.uploadUniform(&shadowPositionTex, "tex", 0);
        quad.draw();
        win.useViewport(3);
        quad_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "tex", 0);
        quad.draw();
        win.useViewport(4);
        quad_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "tex", 0);
        quad.draw();
        win.useViewport(5);
        quad_shader.uploadUniform(shadowDepthTex, "tex", 0);
        quad.draw();
        win.useViewport(6);
        quad_shader.uploadUniform(&lightnessTex, "tex", 0);
        quad.draw();


        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGui::Text(("Mode : " + mode_str).c_str());
            ImGui::SliderInt("Mode", &mode, 0, 4);
            if(mode == 2 || mode == 3 || mode == 4)
                ImGui::SliderInt("PCF Radius", &pcfMaxRadius, 0, 20);
            if(mode == 1 || mode == 3)
                ImGui::SliderInt("Poisson Samples", &numPoissonSamples, 1, 100);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        ImGui::Render();

        bool enableMouse = !io.WantCaptureMouse;

        win.enableMouseNavigation(enableMouse);

        win.swapBuffer();
    }



    return 0;
}
