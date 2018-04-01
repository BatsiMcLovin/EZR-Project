//
// Created by dino on 15.01.16.
//

#include <IO/Window.h>
#include <Renderables/Renderable.h>
#include <Utils/assets.h>
#include <Renderables/Quad.h>
#include <Renderables/Cube.h>
#include <Shaders/SimpleShader.h>
#include <FBO/FBO.h>
#include <Utils/ShaderUtils.h>
#include <Utils/TextureToViewportRenderer.h>
#include <Utils/TextureUtils.h>

int height = 720;
int width = 1280;

int shadow_width = 2000;
int shadow_height = 2000;

glm::vec3 pointLightPos = glm::vec3(-10.687358f, 11.614994f, 10.442025f);

int main()
{

    ezr::Window win("Lighting Demo", width, height, 50, 50, false);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(win.getWindow(), false);

    ezr::Camera camera00;
    camera00.setCamera(glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f)));
    win.setCam(&camera00);

    ezr::Camera camera01;
    camera01.setCamera(glm::lookAt(pointLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    //ASSETS
    std::vector<ezr::Renderable *> cvark_renderables = ezr::assets::createRenderables(
            EZR_PROJECT_PATH "/resources/zwark.dae");
    std::vector<ezr::Renderable *> scene_renderables = ezr::assets::createRenderables(
            EZR_PROJECT_PATH "/resources/demo_scene/demo_scene.dae");

    ezr::Quad quad;
    ezr::Cube skybox;

	auto cubemap = ezr::TextureUtils::loadCubeMapFromPaths(EZR_PROJECT_PATH "/resources/cubemap/00.png",
		EZR_PROJECT_PATH "/resources/cubemap/01.png",
		EZR_PROJECT_PATH "/resources/cubemap/02.png",
		EZR_PROJECT_PATH "/resources/cubemap/03.png",
		EZR_PROJECT_PATH "/resources/cubemap/04.png",
		EZR_PROJECT_PATH "/resources/cubemap/05.png");

    //SHADERS================================

    ezr::SimpleShader gbuffer_tex_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert",
                                                                           EZR_SHADER_PATH "/GBuffer/GBuffer_tex.frag");
    ezr::SimpleShader beautiful_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                                         EZR_SHADER_PATH "/Lighting/Beautiful.frag");
    ezr::SimpleShader ssao_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/AmbientOcclusion/SSAO.frag");
    ezr::SimpleShader depth_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert",
                                                                     EZR_SHADER_PATH "/ShadowMapping/shadowDraw.frag");
    ezr::SimpleShader shadowmap_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                                         EZR_SHADER_PATH "/ShadowMapping/pcfShadowMap_advanced.frag");
    ezr::SimpleShader gbuffer_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert",
                                                                       EZR_SHADER_PATH "/GBuffer/GBuffer.frag");
    ezr::SimpleShader gameboy_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
															    	   EZR_SHADER_PATH "/PostProcessing/GameBoy/gameboy.frag");
	ezr::SimpleShader cubemapping_shader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
																		   EZR_SHADER_PATH "/Cubemap/localCubemap.frag");

	ezr::TextureToViewportRenderer textureToViewportRenderer;


    std::vector<ezr::SimpleShader> scene_shaders;
    for (auto renderable : scene_renderables)
    {
        scene_shaders.push_back(ezr::ShaderUtils::createGBuffer(*renderable));
    }

    //FBO GBUFFER================================

    ezr::FBO fbo_gbuffer(width, height);
    auto fboTextures = ezr::ShaderUtils::createGBufferFBOTextures(&fbo_gbuffer);
    fbo_gbuffer.compile();

    //FBO SSAO================================

    ezr::FBO fbo_ssao(width, height);
    ezr::Texture ssao_tex = fbo_ssao.generateColorBuffer("ssao", 0);
    fbo_ssao.compile();

    //FBO LIGHTING================================

    ezr::FBO fbo_beautiul(width, height);
    ezr::Texture final_beautiful_tex = fbo_beautiul.generateColorBuffer("final", 0);
    fbo_beautiul.generateDepthBuffer("depth");
    fbo_beautiul.compile();

    //FBO DEPTH DRAW================================

    ezr::FBO fbo_depth(shadow_width, shadow_height);

    ezr::Texture position_depth_tex = fbo_depth.generateColorBuffer("position", 0);
    ezr::Texture depth_depth_tex = fbo_depth.generateDepthBuffer("depth");

    fbo_depth.compile();

    //FBO SHADOW MAP================================

    ezr::FBO fbo_shadowmap(width, height);

    ezr::Texture shadowmap_tex = fbo_shadowmap.generateColorBuffer("shadowmap", 0);

    fbo_shadowmap.compile();

	//FBO CUBE MAPPING

	ezr::FBO fbo_cubemapping(width, height);

	ezr::Texture cubemapping_texture = fbo_cubemapping.generateColorBuffer("meh", 0);

	fbo_cubemapping.compile();


    //FBO POST PROCESSING================================

    float gameboy_scalor = 0.2f;
    ezr::FBO fbo_postprocessing(width * gameboy_scalor, height * gameboy_scalor);

    ezr::Texture postprocessing_tex = fbo_postprocessing.generateColorBuffer("pp", 0);

    fbo_postprocessing.compile();

    postprocessing_tex.bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //DEPTH TEXTURE PASS================================

    glm::mat4 depth_perspective = glm::perspective(glm::radians(90.0f), 1.0f, 4.0f, 50.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    fbo_depth.bind();
    win.clear();
    glViewport(0, 0, shadow_width, shadow_height);

    depth_shader.bind();

    depth_shader.uploadUniform(glm::mat4(), "modelMatrix");
    depth_shader.uploadUniform(camera01.getViewMatrix(), "viewMatrix");
    depth_shader.uploadUniform(depth_perspective, "projectionMatrix");

    for (auto c : cvark_renderables)
    {
        c->draw();
    }

    depth_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

    for (auto c : scene_renderables)
    {
        c->draw();
    }


    fbo_depth.unbind();
    glDisable(GL_CULL_FACE);


    //THE BIG WHILE LOOP================================

    glm::mat4 perspective = glm::perspective(glm::radians(40.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
    int gameboy_step_size = 8;
    bool gameboy_use_green = false;

    int mode = 0;

	float roomScale = 15.0f;
	glm::vec3 cubemapOffset = glm::vec3(0.0f, 4.5f, 0.0f);

    while (!win.shouldClose())
    {

        //DRAW STUFF================================

        fbo_gbuffer.bind();
        win.useViewport(0);
        win.clear();

        gbuffer_shader.bind();

        gbuffer_shader.uploadUniform(glm::mat4(), "modelMatrix");
        gbuffer_shader.uploadUniform(perspective, "projectionMatrix");
        gbuffer_shader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");

        for (auto c : cvark_renderables)
        {
            gbuffer_shader.uploadUniform(c->getAttribute(ezr::Matkey_v3::DIFFUSE_COLOR), "diffuseColor");
            gbuffer_shader.uploadUniform(c->getAttribute(ezr::Matkey_v3::SPECULAR_COLOR), "specularColor");
            gbuffer_shader.uploadUniform(c->getAttribute(ezr::Matkey_v1::SHININESS), "specularHardness");

            c->draw();
        }

        gbuffer_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

        gbuffer_tex_shader.bind();

        gbuffer_tex_shader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        gbuffer_tex_shader.uploadUniform(perspective, "projectionMatrix");

        gbuffer_tex_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");
        for (int i = 0; i < scene_renderables.size(); i++)
        {
            auto scene_shader = scene_shaders[i];
            auto scene_renderable = scene_renderables[i];

            scene_shader.bind();

            scene_shader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
            scene_shader.uploadUniform(perspective, "projectionMatrix");
            scene_shader.uploadUniform(glm::scale(glm::vec3(3.0f)), "modelMatrix");

            ezr::ShaderUtils::uploadGBufferMaterialUniforms(scene_shader, *scene_renderable);

            scene_renderable->draw();
        }

        fbo_gbuffer.unbind();

		//CUBEMAPPING
		fbo_cubemapping.bind();
		cubemapping_shader.bind();
		cubemapping_shader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
		cubemapping_shader.uploadUniform(cubemap, "cubemapTexture", 0);
		cubemapping_shader.uploadUniform(fboTextures[ezr::NORMALS_OUTPUT], "normalTexture", 1);
		cubemapping_shader.uploadUniform(fboTextures[ezr::POSITION_OUTPUT], "positionTexture", 2);
		cubemapping_shader.uploadUniform(glm::vec3(roomScale), "roomScale");
		cubemapping_shader.uploadUniform(cubemapOffset, "offset");
		cubemapping_shader.uploadUniform(false, "useFresnel");
		quad.draw();

		

        //SHADOW MAPPING================================

        fbo_shadowmap.bind();
        win.clear();

        shadowmap_shader.bind();
        shadowmap_shader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        shadowmap_shader.uploadUniform(perspective, "projectionMatrix");
        shadowmap_shader.uploadUniform(camera01.getViewMatrix(), "shadowViewMatrix");
        shadowmap_shader.uploadUniform(depth_perspective, "shadowProjMatrix");
        shadowmap_shader.uploadUniform(pointLightPos, "lightPosition");
        shadowmap_shader.uploadUniform(&depth_depth_tex, "shadowTex", 0);
        shadowmap_shader.uploadUniform(&fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
        shadowmap_shader.uploadUniform(&fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 2);

        quad.draw();

        fbo_shadowmap.unbind();

        //SSAO================================

        fbo_ssao.bind();
        win.clear();

        ssao_shader.bind();
        ssao_shader.uploadUniform(&fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 0);
        ssao_shader.uploadUniform(&fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);

        ssao_shader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        ssao_shader.uploadUniform(perspective, "projectionMatrix");

        quad.draw();

        fbo_ssao.unbind();

        //COMPOSITION STUFF================================

        fbo_beautiul.bind();
        win.clear();

        beautiful_shader.bind();

        beautiful_shader.uploadUniform(camera00.getViewMatrix(), "viewMatrix");
        beautiful_shader.uploadUniform(&fboTextures[ezr::NORMALS_OUTPUT], "normalTex", 0);
        beautiful_shader.uploadUniform(&fboTextures[ezr::POSITION_OUTPUT], "positionTex", 1);
        beautiful_shader.uploadUniform(&fboTextures[ezr::DIFFUSE_OUTPUT], "colorTex", 2);
        beautiful_shader.uploadUniform(&fboTextures[ezr::SPECULAR_OUTPUT], "specularTex", 3);
        beautiful_shader.uploadUniform(&ssao_tex, "ambientOcclusionTex", 4);
        beautiful_shader.uploadUniform(&shadowmap_tex, "shadowTex", 5);
		beautiful_shader.uploadUniform(cubemapping_texture, "cubeMapReflectionsTex", 6);
        beautiful_shader.uploadUniform(pointLightPos, "pointLight");

        quad.draw();

        fbo_beautiul.unbind();

        //GAME BOY================================
		if (mode == 8)
		{
			fbo_postprocessing.bind();
			win.clear();
			glViewport(0, 0, width * gameboy_scalor, height * gameboy_scalor);

			gameboy_shader.bind();
			gameboy_shader.uploadUniform(final_beautiful_tex, "tex");
            gameboy_shader.uploadUniform(gameboy_use_green, "gameboy_use_green");
            gameboy_shader.uploadUniform(gameboy_step_size, "steps");
			quad.draw();

			fbo_postprocessing.unbind();
		}

        //DRAW QUADS================================

        win.clear();
        win.useViewport(1);

        switch (mode)
        {
            case 1 : textureToViewportRenderer.renderTexture(fboTextures[ezr::EMISSION_OUTPUT]);
                break;
            case 2 : textureToViewportRenderer.renderTexture(fboTextures[ezr::DEPTH_OUTPUT]);
                break;
            case 3 : textureToViewportRenderer.renderTexture(ssao_tex);
                break;
            case 4 : textureToViewportRenderer.renderTexture(fboTextures[ezr::SPECULAR_OUTPUT]);
				break;
            case 5 : textureToViewportRenderer.renderTexture(position_depth_tex);
				break;
            case 6 : textureToViewportRenderer.renderTexture(fboTextures[ezr::DIFFUSE_OUTPUT]);
				break;
            case 7 : textureToViewportRenderer.renderTexture(fboTextures[ezr::NORMALS_OUTPUT]);
				break;
			case 8: textureToViewportRenderer.renderTexture(postprocessing_tex);
				break;
			case 9: textureToViewportRenderer.renderTexture(cubemapping_texture);
				break;
            default: textureToViewportRenderer.renderTexture(final_beautiful_tex);
				break;
        }
        quad.draw();

        win.useViewport(2);
		textureToViewportRenderer.renderTexture(&ssao_tex);

        win.useViewport(3);
		textureToViewportRenderer.renderTexture(&fboTextures[ezr::SPECULAR_OUTPUT]);

        win.useViewport(4);
		textureToViewportRenderer.renderTexture(&position_depth_tex);

        win.useViewport(5);
		textureToViewportRenderer.renderTexture(&fboTextures[ezr::DIFFUSE_OUTPUT]);

        win.useViewport(6);
		textureToViewportRenderer.renderTexture(&fboTextures[ezr::NORMALS_OUTPUT]);


        // GUI STUFF
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SliderInt("Mode", &mode, 0, 9);
            if (mode == 8)
            {
                ImGui::Checkbox("Use GAMEBOY green", &gameboy_use_green);
                ImGui::SliderInt("Steps", &gameboy_step_size, 1, 16);
            }
			ImGui::SliderFloat("CM local scale", &roomScale, 1.0f, 20.0f);
			ImGui::SliderFloat3("CM local offset", &cubemapOffset.x, -5.0f, 5.0f);

            ImGui::Text("Average %.3f ms/frame \n(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        }

        ImGui::Render();

        bool enableMouse = !io.WantCaptureMouse;

        win.enableMouseNavigation(enableMouse);

        win.swapBuffer();
    }
    return 0;
}
