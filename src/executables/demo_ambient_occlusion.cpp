//
// Created by dino on 15.01.16.
//

#include <IO/Window.h>
#include <Renderables/Cube.h>
#include <Renderables/Quad.h>
#include <Utils/assets.h>
#include <Shaders/SimpleShader.h>
#include <FBO/FBO.h>
#include <Utils/ShaderUtils.h>

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
    ezr::Window win("Ambient Occlusion Demo", width, height, 10, 10, false);
    ezr::Camera cam;
    cam.setCamera(glm::lookAt(glm::vec3(1.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0)));
    win.setCam(&cam);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(win.getWindow(), false);

    //Assets
    ezr::Cube cube;
    ezr::Quad quad;
    auto cvark_renderables = ezr::assets::createRenderables(EZR_PROJECT_PATH "/resources/zwark.dae");
    auto scene_renderables = ezr::assets::createRenderables(EZR_PROJECT_PATH "/resources/demo_scene/demo_scene.dae");

    //Shaders

    ezr::SimpleShader gbuffer_shader(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/GBuffer.frag");
    ezr::SimpleShader gbuffer_tex_shader(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/GBuffer_tex.frag");
    ezr::SimpleShader ssao_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/AmbientOcclusion/SSAO.frag");
    ezr::SimpleShader ssao_blur_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/AmbientOcclusion/blur.frag");
    ezr::SimpleShader quad_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");
    ezr::SimpleShader multiply_shader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/Misc/multiply.frag");

    std::vector<ezr::SimpleShader> scene_shaders;
    for (auto renderable : scene_renderables)
    {
        scene_shaders.push_back(ezr::ShaderUtils::createGBuffer(*renderable));
    }

    //Texture
    ezr::Texture space_tex(EZR_PROJECT_PATH "/resources/space.jpg");

    //FBO GBuffer
    ezr::FBO fbo_gbuffer(width, height);
    auto fboTextures = ezr::ShaderUtils::createGBufferFBOTextures(&fbo_gbuffer);
    fbo_gbuffer.compile();

    //FBO SSAO
    ezr::FBO fboSSAO(width, height);

    ezr::Texture ssao_tex = fboSSAO.generateColorBuffer("ssao", 0);

    fboSSAO.compile();

    //FBO BLUR
    ezr::FBO fboBlur(width, height);
    ezr::Texture blurred_tex = fboBlur.generateColorBuffer("blurred", 0);

    fboBlur.compile();

    //FBO MULTIPLY
    ezr::FBO fboMultiply(width, height);

    ezr::Texture final_tex = fboMultiply.generateColorBuffer("final", 0);

    fboMultiply.compile();

    //Matrices
    glm::mat4 proj_mat = glm::perspective(glm::radians(60.0f), static_cast<float>(width)/height, 0.1f, 100.0f);
    float t = 0.0f;


    while(!win.shouldClose())
    {

        //CVARK MAT
        glm::mat4 cvark_mat = glm::rotate(t, glm::vec3(0.0f, 1.0f ,0.0f)) * glm::scale(glm::vec3(0.25f));
        t += 0.01f;

        //FIRST PASS (DRAW STUFF)
        fbo_gbuffer.bind();
        win.clear(glm::vec3(0.25f));
        win.useViewport(0);
        gbuffer_shader.bind();

        gbuffer_shader.uploadUniform(proj_mat, "projectionMatrix");
        gbuffer_shader.uploadUniform(cam.getViewMatrix(), "viewMatrix");

        gbuffer_shader.uploadUniform(cvark_mat, "modelMatrix");
        for(auto c : cvark_renderables)
        {
            gbuffer_shader.uploadUniform(c->getAttribute(ezr::Matkey_v3::DIFFUSE_COLOR), "diffuseColor");

            c->draw();
        }

        gbuffer_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

        gbuffer_tex_shader.bind();

        gbuffer_tex_shader.uploadUniform(proj_mat, "projectionMatrix");
        gbuffer_tex_shader.uploadUniform(cam.getViewMatrix(), "viewMatrix");
        gbuffer_tex_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

        for (int i = 0; i < scene_renderables.size(); i++)
        {
            auto scene_shader= scene_shaders[i];
            auto scene_renderable = scene_renderables[i];

            scene_shader.bind();

            scene_shader.uploadUniform(cam.getViewMatrix(), "viewMatrix");
            scene_shader.uploadUniform(proj_mat, "projectionMatrix");
            scene_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

            ezr::ShaderUtils::uploadGBufferMaterialUniforms(scene_shader, *scene_renderable);

            scene_renderable->draw();
        }

        fbo_gbuffer.unbind();

        //SECOND PASS (SSAO)

        fboSSAO.bind();
        win.clear();

        ssao_shader.bind();
        ssao_shader.uploadUniform(num_samples, "num_samples");
        ssao_shader.uploadUniform(border_threshold, "border_threshold");
        ssao_shader.uploadUniform(contrast, "contrast");
        ssao_shader.uploadUniform(radius, "radius");
        ssao_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 0);
        ssao_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
        ssao_shader.uploadUniform(cam.getViewMatrix(), "viewMatrix");
        ssao_shader.uploadUniform(proj_mat, "projectionMatrix");

        quad.draw();

        fboSSAO.unbind();


        fboBlur.bind();
        win.clear();

        ssao_blur_shader.bind();
        ssao_blur_shader.uploadUniform(&fboTextures[ezr::DEPTH_OUTPUT], "depthTex", 0);
        ssao_blur_shader.uploadUniform(&ssao_tex, "aoTex", 1);

        quad.draw();

        fboBlur.unbind();

        fboMultiply.bind();
        win.clear();

        multiply_shader.bind();
        multiply_shader.uploadUniform(&blurred_tex, "texA", 0);
        multiply_shader.uploadUniform(&fboTextures[ezr::DIFFUSE_OUTPUT], "texB", 1);

        quad.draw();

        fboMultiply.unbind();

        win.clear();

        quad_shader.bind();


        win.useViewport(2);
        quad_shader.uploadUniform(&fboTextures[ezr::DIFFUSE_OUTPUT], "tex", 0);
        quad.draw();

        win.useViewport(3);
        quad_shader.uploadUniform(&space_tex, "tex", 0);
        quad.draw();

        win.useViewport(4);
        quad_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "tex", 0);
        quad.draw();

        win.useViewport(5);
        quad_shader.uploadUniform(fboTextures[ezr::DEPTH_OUTPUT], "tex", 0);
        quad.draw();

        win.useViewport(6);
        quad_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "tex", 0);
        quad.draw();

        win.useViewport(1);
        if(withColor)
            quad_shader.uploadUniform(&final_tex, "tex", 0);
        else
            quad_shader.uploadUniform(&blurred_tex, "tex", 0);
        quad.draw();


        // GUI STUFF
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::Checkbox("With Color", &withColor);
            ImGui::SliderInt("Num of Samples", &num_samples, 1, 100);
            ImGui::SliderFloat("Border Threshold", &border_threshold, 0.0f, 10.0f);
            ImGui::SliderFloat("Sample Radius", &radius, 0.01f, 5.0f);
            ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);

            ImGui::Text("Average %.3f ms/frame \n(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        ImGui::Render();

        bool enableMouse = !io.WantCaptureMouse;

        win.enableMouseNavigation(enableMouse);

        win.swapBuffer();
    }
    return 0;
}
