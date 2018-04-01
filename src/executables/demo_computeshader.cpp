//
// Created by rheidrich on 19.05.17.
//

#include <IO/Window.h>
#include <FBO/Texture.h>
#include <Renderables/Quad.h>
#include <Shaders/SimpleShader.h>
#include <Utils/ShaderUtils.h>
#include <Shaders/ComputeShader.h>
#include <SSBO/SSBO.h>


std::vector<float> getRandomVector(int i);

void printVec(std::vector<float> vector);

int main()
{
    ezr::Window window("Compute Shader Demo", 600, 400);
    ezr::Texture tex00(EZR_PROJECT_PATH "/resources/best.jpg");
    ezr::Texture tex01(EZR_PROJECT_PATH "/resources/best.jpg");
    ezr::Quad quad;

    ezr::SimpleShader quadShader = ezr::ShaderUtils::loadFromPaths(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");

    //####### SSBO COMPUTE SHADER ###########

    ezr::ComputeShader computeShader_ssbo = ezr::ShaderUtils::loadComputeShaderFromPath(EZR_SHADER_PATH "/ComputeShader/Test/computeShaderTest.comp");
    computeShader_ssbo.bind();

    auto randVec = getRandomVector(100);
    printVec(randVec);
    ezr::SSBO ssbo00(randVec, 0);
    ezr::SSBO ssbo01(randVec, 1);

    ssbo00.bind(0);
    ssbo01.bind(1);
    computeShader_ssbo.uploadUniform(1.1f, "multiplicator");
	tex00.bindImageTexture(0);
    computeShader_ssbo.uploadUniform(0, "testImage");
    computeShader_ssbo.uploadUniform(tex00, "testImage");
	computeShader_ssbo.dispatch(100, 1, 1);

    printVec(ssbo01.downloadData<float>());


    //####### IMG COMPUTE SHADER ###########

    ezr::ComputeShader computeShader_img = ezr::ShaderUtils::loadComputeShaderFromPath(EZR_SHADER_PATH "/ComputeShader/Test/computeShaderTest_img2D.comp");
    computeShader_img.bind();

    ezr::debug::println(tex00.getSize());

	tex00.bindImageTexture(1);
	tex01.bindImageTexture(0);
	computeShader_img.dispatch((GLuint) tex00.getSize().x, (GLuint) tex00.getSize().y, 1);
	computeShader_img.memoryBarrier();

    //####### MAIN LOOP ###########

    while(!window.shouldClose())
    {
        window.clear();

        quadShader.bind();
        quadShader.uploadUniform(tex01, "tex", 0);
        quad.draw();
        window.swapBuffer();
    }

    return 0;
}

void printVec(std::vector<float> vector)
{
    for (int i = 0; i < vector.size(); ++i)
    {
        ezr::debug::print(vector[i]);
        ezr::debug::print(", ");
    }

    ezr::debug::println(" thats it");
}

std::vector<float> getRandomVector(int size)
{
    std::vector<float> vec;
    vec.resize(size);

    for (int i = 0; i < size; ++i)
    {
        vec[i] = (std::rand() % 100) + 1;
    }
    return vec;
}