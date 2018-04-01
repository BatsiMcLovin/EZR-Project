//
// Created by rheidrich on 26.05.17.
//

#include <IO/Camera.h>
#include <IO/Window.h>
#include <Renderables/Renderable.h>
#include <Utils/assets.h>
#include <Renderables/Quad.h>
#include <Utils/ShaderUtils.h>
#include <Utils/TextureUtils.h>
#include <Utils/TextureToViewportRenderer.h>

int height = 720;
int width = 1280;

int main()
{

	// WINDOW
	ezr::Window window("Cube Map Demo", width, height);
	ezr::Camera camera;
	camera.setCamera(glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f)));
	window.setCam(&camera);

	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window.getWindow(), false);

	//ASSETS
	std::vector<ezr::Renderable *> cvark_renderables = ezr::assets::createRenderables(
		EZR_PROJECT_PATH "/resources/zwark.dae");
	std::vector<ezr::Renderable *> scene_renderables = ezr::assets::createRenderables(
		EZR_PROJECT_PATH "/resources/demo_scene/demo_scene.dae");


	std::vector<ezr::Renderable *> renderables = scene_renderables;
	renderables.insert(renderables.end(), cvark_renderables.begin(), cvark_renderables.end());

	ezr::Quad quad;


	ezr::TextureData texData;
	texData.data = new unsigned char[27]{	255, 0, 0, 
											0, 0, 255, 
											255, 255, 255, 
											0, 255, 0,
											255, 0, 0,
											0, 0, 255,
											255, 255, 255,
											0, 255, 0,
											0, 255, 0 };
	texData.width = 3;
	texData.height = 3;
	texData.bytesPerPixel = 3;

	std::vector<ezr::TextureData> texDatas;
	texDatas.push_back(texData);
	texDatas.push_back(texData);
	texDatas.push_back(texData);
	texDatas.push_back(texData);
	texDatas.push_back(texData);
	texDatas.push_back(texData);

		
	auto cubemap = ezr::TextureUtils::loadCubeMapFromPaths(EZR_PROJECT_PATH "/resources/cubemap/00.png",
		EZR_PROJECT_PATH "/resources/cubemap/01.png",
		EZR_PROJECT_PATH "/resources/cubemap/02.png",
		EZR_PROJECT_PATH "/resources/cubemap/03.png",
		EZR_PROJECT_PATH "/resources/cubemap/04.png",
		EZR_PROJECT_PATH "/resources/cubemap/05.png");

	ezr::Texture carl(ezr::TextureUtils::loadFromFile(EZR_PROJECT_PATH "/resources/best.jpg"));

	//SHADERS
	auto gbuffers = ezr::ShaderUtils::createGBuffers(renderables);
	ezr::SimpleShader cubemapShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
		EZR_SHADER_PATH "/Cubemap/simpleCubemap.frag");
    ezr::SimpleShader localCubemapShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                                           EZR_SHADER_PATH "/Cubemap/localCubemap.frag");
    ezr::SimpleShader addShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert",
                                                                           EZR_SHADER_PATH "/Misc/add.frag");
	ezr::TextureToViewportRenderer textureToViewportRenderer;
    //FBOs
	ezr::FBO fboGBuffer(width, height);
	auto gbufferTextures = ezr::ShaderUtils::createGBufferFBOTextures(&fboGBuffer);
	fboGBuffer.compile();

    ezr::FBO fboCubeMap(width, height);
    auto cubeMapTex = fboCubeMap.generateColorBuffer("color", 0);
    fboCubeMap.generateDepthBuffer();
    fboCubeMap.compile();

    ezr::FBO fboFinal(width, height);
    auto finalTex = fboFinal.generateColorBuffer("color", 0);
    fboFinal.generateDepthBuffer();
    fboFinal.compile();

    //VARIABLES
	glm::mat4 perspective = glm::perspective(glm::radians(40.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
	int mode = 0;
    bool useFresnel = true;

	float cvarkPos[3] = { 0.0f, 0.0f, 0.0f };
    float roomScale = 5.0f;

	while (!window.shouldClose())
	{

		for (auto r : cvark_renderables)
		{
			r->_scale = glm::vec3(0.25f);
			r->_position = glm::vec3(cvarkPos[0], cvarkPos[1], cvarkPos[2]);
		}

		window.clear();

		fboGBuffer.bind();
		window.clear();
		for (int i = 0; i < renderables.size(); ++i)
		{
			auto shader = gbuffers[i];
			ezr::Renderable renderable = *renderables[i];
			shader.bind();
			shader.uploadUniform(renderable.getModelMatrix(), "modelMatrix");
			shader.uploadUniform(camera.getViewMatrix(), "viewMatrix");
			shader.uploadUniform(perspective, "projectionMatrix");
			ezr::ShaderUtils::uploadGBufferMaterialUniforms(shader, renderable);

			renderable.draw();
		}


		fboGBuffer.unbind();

		// CUBEMAP PASS
		fboCubeMap.bind();
		window.clear();

		ezr::SimpleShader cmShader;

		switch (mode)
		{
		case 1:
        case 3:
			cmShader = localCubemapShader;
			break;
		default:
        case 2:
			cmShader = cubemapShader;
			break;
		}

		cmShader.bind();

		cmShader.uploadUniform(camera.getViewMatrix(), "viewMatrix");
		cmShader.uploadUniform(cubemap, "cubemapTexture", 0);
		cmShader.uploadUniform(gbufferTextures[ezr::NORMALS_OUTPUT], "normalTexture", 1);
		cmShader.uploadUniform(gbufferTextures[ezr::POSITION_OUTPUT], "positionTexture", 2);
        cmShader.uploadUniform(useFresnel, "useFresnel");
        cmShader.uploadUniform(glm::vec3(roomScale), "roomScale");
		quad.draw();

		fboCubeMap.unbind();
        fboFinal.bind();
        window.clear();

        addShader.bind();
        addShader.uploadUniform(cubeMapTex, "texA", 0);
        addShader.uploadUniform(gbufferTextures[ezr::DIFFUSE_OUTPUT], "texB", 1);
        quad.draw();


        fboFinal.unbind();
        window.clear();


        ezr::Texture viewTex = cubeMapTex;
        if(mode > 1)
            viewTex = finalTex;

		window.useViewport(1);
        textureToViewportRenderer.renderTexture(viewTex);

		window.useViewport(2);
        textureToViewportRenderer.renderTexture(gbufferTextures[ezr::POSITION_OUTPUT]);

        window.useViewport(3);
        textureToViewportRenderer.renderTexture(gbufferTextures[ezr::NORMALS_OUTPUT]);

        window.useViewport(4);
        textureToViewportRenderer.renderTexture(gbufferTextures[ezr::DEPTH_OUTPUT]);

        window.useViewport(5);
        textureToViewportRenderer.renderTexture(gbufferTextures[ezr::DIFFUSE_OUTPUT]);

        window.useViewport(6);
        textureToViewportRenderer.renderTexture(carl);

		//IMGUI


		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplGlfwGL3_NewFrame();
		{
			float max = 5.0f;
			ImGui::SliderInt("Mode", &mode, 0, 3);
            ImGui::Checkbox("Use Fresnel", &useFresnel);
			ImGui::SliderFloat3("Position", cvarkPos, -max, max);
            ImGui::SliderFloat("Room Scale", &roomScale, 0.1, 10.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::Render();

		bool enableMouse = !io.WantCaptureMouse;

		window.enableMouseNavigation(enableMouse);


		//SWAP BUFFER

		window.swapBuffer();
	}
	return 0;
}