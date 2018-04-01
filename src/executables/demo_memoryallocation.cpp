//
// Created by rheidrich on 21.06.17.
//


#include <IO/Window.h>
#include <FBO/Texture.h>
#include <Utils/TextureUtils.h>
#include <Shaders/ComputeShader.h>
#include <Utils/ShaderUtils.h>
#include <Utils/assets.h>
#include <SSBO/SSBO.h>
#include <Utils/TextureToViewportRenderer.h>

int height = 720;
int width = 1280;

int main()
{
    // WINDOW
    ezr::Window window("Cube Map Demo", width, height);
    ImGui_ImplGlfwGL3_Init(window.getWindow(), false);

    ezr::Texture textureA = ezr::TextureUtils::loadFromFile(EZR_PROJECT_PATH "/resources/best.jpg");
    ezr::Cubemap cubemapA = ezr::TextureUtils::loadCubeMapFromPaths(EZR_PROJECT_PATH "/resources/cubemap/00.png",
                                                                    EZR_PROJECT_PATH "/resources/cubemap/01.png",
                                                                    EZR_PROJECT_PATH "/resources/cubemap/02.png",
                                                                    EZR_PROJECT_PATH "/resources/cubemap/03.png",
                                                                    EZR_PROJECT_PATH "/resources/cubemap/04.png",
                                                                    EZR_PROJECT_PATH "/resources/cubemap/05.png");

    std::vector<ezr::Renderable *> cvark_renderables = ezr::assets::createRenderables(
            EZR_PROJECT_PATH "/resources/zwark.dae");

    ezr::ComputeShader computeShader = ezr::ShaderUtils::loadComputeShaderFromPath(EZR_SHADER_PATH "/ComputeShader/Test/computeShaderTest.comp");
    ezr::SimpleShader simpleShader = ezr::ShaderUtils::createGBuffer(*cvark_renderables[0]);

    ezr::TextureToViewportRenderer textureToViewportRenderer;

    std::vector<float> data;
    data.resize(100000000, 999.0f);
    ezr::SSBO ssbo(data, 0);

    ezr::FBO fbo(width, height);
    ezr::ShaderUtils::createGBufferFBOTextures(&fbo);
    fbo.compile();

    while(!window.shouldClose())
    {
        window.clear();

        textureToViewportRenderer.renderTexture(textureA);

        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfwGL3_NewFrame();
        {
            if(ImGui::Button("Renderable"))
            {
                for(auto r : cvark_renderables)
                {
                    r->free();
                    ezr::debug::glErrorPrintln();
                }
                ezr::debug::println("Free Renderables");
            }
            if(ImGui::Button("Texture"))
            {

                textureA.free();
                ezr::debug::println("Free Texture");
            }
            if(ImGui::Button("Cubemap"))
            {
                cubemapA.free();
                ezr::debug::println("Free Cubemap");
            }
            if(ImGui::Button("ComputeShader"))
            {
                computeShader.free();
                ezr::debug::println("Free Compute Shader");
            }
            if(ImGui::Button("Shader"))
            {
                simpleShader.free();
                ezr::debug::println("Free Shader");
            }
            if(ImGui::Button("SSBO"))
            {
                ssbo.free();
                ezr::debug::println("Free SSBO");

            }
            if(ImGui::Button("FBO"))
            {
                fbo.free(true);
                ezr::debug::println("Free FBO");
            }
            if(ImGui::Button("PENSI"))
            {
                ezr::SSBO ssbo00(data, 0);
                ssbo00.free();
            }
        }
        ImGui::Render();

        window.swapBuffer();
    }


    return 0;
}