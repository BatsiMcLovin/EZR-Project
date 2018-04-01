#include <IO/Window.h>
#include <Renderables/Renderable.h>
#include <Utils/assets.h>
#include <Renderables/Quad.h>
#include <Renderables/Cube.h>
#include <Shaders/SimpleShader.h>
#include <FBO/FBO.h>
#include <SSBO/SSBO.h>
#include <AntiAliasing/MLAA.h>

int width = 1400;
int height = 1000;

int max_expected_neighbors = 40;

ezr::SimpleShader* neighborhoodShader = 0;
std::vector<ezr::SSBO> ssboNeighborList;
std::vector<ezr::SSBO> ssboPositionList;
std::vector<ezr::SSBO> ssboNormalList;
std::vector<ezr::SSBO> ssboCurvList;
std::vector<ezr::SSBO> ssboDerivList;


// ///////////////////
// CALCULATE NEIGHBORS
// ///////////////////

ezr::SSBO calcNeighbors(ezr::Renderable* renderable)
{

    std::vector<int> ssboVec(renderable->getIndexCount() * max_expected_neighbors, 0);
    ezr::SSBO ssboNeighbors(ssboVec);

    std::vector<int> zeroVec(renderable->getIndexCount() * max_expected_neighbors, 0);
    ezr::SSBO ssboAtomicCounter(zeroVec, 1);

    std::vector<glm::vec4> posvecs(renderable->getVertexCount() * max_expected_neighbors, glm::vec4(0));
    ezr::SSBO ssboPositions(posvecs, 2);

    std::vector<glm::vec4> normvecs(renderable->getVertexCount() * max_expected_neighbors, glm::vec4(0));
    ezr::SSBO ssboNormal(normvecs, 3);

    std::vector<glm::vec4> curvvecs(renderable->getVertexCount() * max_expected_neighbors, glm::vec4(0));
    ezr::SSBO ssboCurv(curvvecs, 4);

    std::vector<float> derivvecs(renderable->getVertexCount() * max_expected_neighbors, 0.0f);
    ezr::SSBO ssboDeriv(derivvecs, 5);



    if(neighborhoodShader != 0)
    {
        neighborhoodShader->bind();
        ssboNeighbors.bind(0);
        ssboAtomicCounter.bind(1);
        ssboPositions.bind(2);
        ssboNormal.bind(3);
        renderable->draw();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    ssboNeighborList.push_back(ssboNeighbors);  // Neighbors
    ssboPositionList.push_back(ssboPositions);  // Positions
    ssboNormalList.push_back(ssboNormal);       // Normals
    ssboCurvList.push_back(ssboCurv);           // Curvature
    ssboDerivList.push_back(ssboDeriv);         // Derivative

    return ssboNeighbors;
}

// ///////////////////
// CREATE CVARK ASSETS
// ///////////////////

std::vector<ezr::Renderable*> createSmoothedMeshAndSSBO(std::string path)
{
    // LOAD ASSETS

    auto scene = ezr::assets::import(path);
    scene = ezr::assets::removeDoublesAndSmooth(scene);
    std::vector<ezr::Renderable*> renderablesList = ezr::assets::createRenderables(scene);

    // CALCULATE NEIGHBORHOOD

    for(auto renderable : renderablesList)
    {
        calcNeighbors(renderable);

    }


    return renderablesList;
}


// ///////////////////
// MAIN METHOD
// ///////////////////

int main()
{
    // //////////////////////////////////////
    // INITIALIZATION

    ezr::Window win("MedCV", width, height, 100, 100, false);
    ezr::Camera camera00;
    camera00.setCamera(glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f)));
    win.setCam(&camera00);

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(win.getWindow(), false);

    // SHADERS
    ezr::SimpleShader quadShader(EZR_SHADER_PATH "/GBuffer/simplePass.vert", EZR_SHADER_PATH "/GBuffer/simpleTexture.frag");
    ezr::SimpleShader gbufferShader(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert", EZR_SHADER_PATH "/GBuffer/GBuffer.frag");
    neighborhoodShader = new ezr::SimpleShader(EZR_SHADER_PATH "/medcv/01/neighborSearch.vert", EZR_SHADER_PATH "/medcv/01/neighborSearch.geom", EZR_SHADER_PATH "/medcv/01/neighborSearch.frag");
    ezr::SimpleShader contourShader(EZR_SHADER_PATH "/medcv/02/contourDepth.vert", EZR_SHADER_PATH "/medcv/02/contourDepth.geom", EZR_SHADER_PATH "/medcv/02/contourDepth.frag");
    ezr::SimpleShader gradientShader(EZR_SHADER_PATH "/medcv/03/computeGradient.vert", EZR_SHADER_PATH "/medcv/03/computeGradient.frag");
    ezr::SimpleShader derivationShader(EZR_SHADER_PATH "/medcv/04/deriveGradient.vert", EZR_SHADER_PATH "/medcv/04/deriveGradient.frag");
    ezr::SimpleShader prepareLicShader(EZR_SHADER_PATH "/medcv/05/prepareLIC.vert", EZR_SHADER_PATH "/medcv/05/prepareLIC.geom", EZR_SHADER_PATH "/medcv/05/prepareLIC.frag");
    ezr::SimpleShader licShader(EZR_SHADER_PATH "/medcv/06/LIC.vert", EZR_SHADER_PATH "/medcv/06/LIC.frag");

    // TEXTURES
    ezr::FBO ssboFBO(width, height);
    ezr::Texture ssboTexture = ssboFBO.generateColorBuffer("ssboOutput", 0);
    ssboFBO.generateDepthBuffer("depth");
    ssboFBO.compile();

    ezr::FBO contourFBO(width, height);
    ezr::Texture contourTexture = contourFBO.generateColorBuffer("contourOutput", 0);
    ezr::Texture contourDepthTexture = contourFBO.generateDepthBuffer("depth");
    contourFBO.compile();

    ezr::FBO gradientFBO(width, height);
    ezr::Texture gradientTexture00 = gradientFBO.generateColorBuffer("fragcolor", 0);
    ezr::Texture gradientTexture01 = gradientFBO.generateColorBuffer("fragcolorPosOutput", 1);
    gradientFBO.generateDepthBuffer("depth");
    gradientFBO.compile();

    ezr::FBO derivationFBO(width, height);
    ezr::Texture derivationTexture = derivationFBO.generateColorBuffer("fragcolor", 0);
    derivationFBO.generateDepthBuffer("depth");
    derivationFBO.compile();

    ezr::FBO prepareLicFBO(width, height);
    ezr::Texture prepareLicTexture00 = prepareLicFBO.generateColorBuffer("fragcolor", 0);
    ezr::Texture prepareLicTexture01 = prepareLicFBO.generateColorBuffer("fragcolor", 1);
    ezr::Texture prepareLicTexture02 = prepareLicFBO.generateColorBuffer("fragcolor", 2);
    ezr::Texture prepareLicTexture03 = prepareLicFBO.generateColorBuffer("fragcolor", 3);
    ezr::Texture prepareLicDepthTexture = prepareLicFBO.generateDepthBuffer("depth");
    prepareLicFBO.compile();

    ezr::FBO licFBO(width, height);
    ezr::Texture licTexture00 = licFBO.generateColorBuffer("fragcolor", 0);
    licFBO.generateDepthBuffer("depth");
    licFBO.compile();

    ezr::FBO lineConnectionFBO(width, height);
    ezr::Texture lineConnectionTexture00 = lineConnectionFBO.generateColorBuffer("fragcolor", 0);
    lineConnectionFBO.generateDepthBuffer("depth");
    lineConnectionFBO.compile();

    ezr::Texture tex(EZR_PROJECT_PATH "/resources/best.jpg");

    // MISC
    ezr::Quad quad;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(40.0f), float(width)/height, 0.1f, 100.0f);

    // ASSETS
    std::vector<ezr::Renderable*> cvarkRenderables = createSmoothedMeshAndSSBO(EZR_PROJECT_PATH "/resources/zwark.dae");

    // LOOP VARIABLES
    bool canStart = gbufferShader.isOK()&&
            quadShader.isOK()           &&
            neighborhoodShader->isOK()  &&
            contourShader.isOK();


    float lineWidth = 0.01f;
    float lineFlatness = 0.0f;
    float lineColor = 0.00001f;

    int mode = 0;
    glm::vec3 phonglightDir(1.0f);

    int snapGradientAmount = 0;

    float radial_distance_thresh = 0.5f;
    float radialDistanceReverseTransition = 0.1f;
    float outerStrokeColor = 1.0f;
    float input_dotRad = 0.01f;

    float wingRange = 0.25;
    float permeabilityFact = 4;
    float blendCurvatureFact = 1;

    float phongTransition = 0.5f;
    float featureSize = 1.0f;
    float coreSize = 0.5f;

    glm::vec2 screenSize = glm::vec2(width, height);
    int LICIteration = 15;
    float stepFactor = 2;
    float lookUpDist = 2;
    float viewSpaceMaxDistance = 0.018f;
    float licContrast = 2.0f;
    float licAngle = 0.0f;
    float secondLicAngle = 0.0f;
    float smoothFactor = 0.0f;

    bool viewSpaceDistance = true;

    bool stipplingLineConnection = true;

    // //////////////////////////////////////
    // MAIN LOOP
    while(!win.shouldClose() && canStart)
    {
        glm::mat4 viewMatrix = camera00.getViewMatrix();
        win.clear();

        // CONTOUR WITH DEPTH
        contourFBO.bind();
        win.clear(glm::vec3(0));

        contourShader.bind();

        contourShader.uploadUniform(viewMatrix, "viewMatrix");
        contourShader.uploadUniform(projectionMatrix, "projectionMatrix");
        contourShader.uploadUniform(lineWidth, "lineWidth");
        contourShader.uploadUniform(lineFlatness, "lineFlatness");

        for(int i = 0; i< cvarkRenderables.size(); i++)
        {
            ssboNeighborList[i].bind(0);
            ssboPositionList[i].bind(2);
            cvarkRenderables[i]->draw();
        }

        // LIGHT GRADIENT
        gradientFBO.bind();
        win.clear(glm::vec3(1));

        gradientShader.bind();

        gradientShader.uploadUniform(viewMatrix, "viewMatrix");
        gradientShader.uploadUniform(projectionMatrix, "projectionMatrix");

        for(int i = 0; i< cvarkRenderables.size(); i++)
        {
            ssboNeighborList[i].bind(0);
            ssboPositionList[i].bind(2);
            ssboNormalList[i].bind(3);
            ssboCurvList[i].bind(4);
            cvarkRenderables[i]->draw();
        }

        // DERIVE LIGHT GRADIENT

        derivationFBO.bind();
        win.clear(glm::vec3(0));

        derivationShader.bind();

        derivationShader.uploadUniform(viewMatrix, "viewMatrix");
        derivationShader.uploadUniform(projectionMatrix, "projectionMatrix");

        for(int i = 0; i< cvarkRenderables.size(); i++)
        {
            ssboNeighborList[i].bind(0);
            ssboPositionList[i].bind(2);
            ssboNormalList[i].bind(3);
            ssboCurvList[i].bind(4);
            ssboDerivList[i].bind(5);
            cvarkRenderables[i]->draw();
        }

        // PREPARE HATCHING

        prepareLicFBO.bind();
        win.clear(glm::vec3(1));

        prepareLicShader.bind();

        prepareLicShader.uploadUniform(viewMatrix, "viewMatrix");
        prepareLicShader.uploadUniform(projectionMatrix, "projectionMatrix");

        prepareLicShader.uploadUniform(radialDistanceReverseTransition, "radialDistanceReverseTransition");
        prepareLicShader.uploadUniform(outerStrokeColor, "outerStrokeColor");
        prepareLicShader.uploadUniform( input_dotRad, "input_dotRad");
        prepareLicShader.uploadUniform( wingRange, "wingRange");
        prepareLicShader.uploadUniform(coreSize, "coreSize");
        prepareLicShader.uploadUniform( permeabilityFact, "permeabilityFact");
        prepareLicShader.uploadUniform( blendCurvatureFact, "blendCurvatureFact");
        prepareLicShader.uploadUniform( phongTransition, "phongTransition");
        prepareLicShader.uploadUniform( featureSize, "featureSize");
        prepareLicShader.uploadUniform(phonglightDir, "lightDirection");
        prepareLicShader.uploadUniform(snapGradientAmount, "snapGradientAmount");

        for(int i = 0; i< cvarkRenderables.size(); i++)
        {
            ssboNeighborList[i].bind(0);
            ssboPositionList[i].bind(2);
            ssboNormalList[i].bind(3);
            ssboCurvList[i].bind(4);
            ssboDerivList[i].bind(5);
            cvarkRenderables[i]->draw();
        }

        // HATCHING

        //MIPMAP THE SEED TEXTURE

        prepareLicTexture00.bind();
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        prepareLicTexture00.unbind();


        licFBO.bind();
        win.clear(glm::vec3(0,1,1));

        licShader.bind();

        licShader.uploadUniform(prepareLicTexture00, "colorFrontFaces", 0);
        licShader.uploadUniform(prepareLicTexture01, "vectorData", 2);
        licShader.uploadUniform(prepareLicTexture02, "phongTexture", 4);
        licShader.uploadUniform(contourTexture, "contour", 5);
        licShader.uploadUniform(contourDepthTexture, "depthContour", 6);
        licShader.uploadUniform(prepareLicDepthTexture, "depthTexture", 7);
        licShader.uploadUniform(prepareLicTexture03, "voronoiTexture", 8);

        licShader.uploadUniform(screenSize, "screenSize");
        licShader.uploadUniform(projectionMatrix, "projectionMatrix");
        licShader.uploadUniform(LICIteration, "LICIteration");
        licShader.uploadUniform(stepFactor, "stepFactor");
        licShader.uploadUniform(lookUpDist, "lookUpDist");
        licShader.uploadUniform(lineColor, "lineColor");
        licShader.uploadUniform(licContrast, "licContrast");
        licShader.uploadUniform(viewSpaceDistance, "viewSpaceDistance");
        licShader.uploadUniform(viewSpaceMaxDistance, "viewSpaceMaxDistance");
        licShader.uploadUniform(licAngle, "licAngle");
        licShader.uploadUniform(secondLicAngle, "secondLicAngle");
        licShader.uploadUniform(stipplingLineConnection, "stipplingLineConnection");
        licShader.uploadUniform( input_dotRad, "input_dotRad");
        prepareLicShader.uploadUniform( wingRange, "wingRange");
        licShader.uploadUniform(smoothFactor, "smoothFactor");


        quad.draw();

        // COMPOSE

        ezr::Texture* viewPortTex = &ssboTexture;
        std::string modeName;

        switch(mode)
        {
        case 0 :
            viewPortTex = &licTexture00;
            modeName = "Final LIC";
            break;
        case 1 :
            viewPortTex = &prepareLicTexture03;
            modeName = "prepare LIC output 3";
            break;
        case 2 :
            viewPortTex = &prepareLicTexture00;
            modeName = "prepare LIC output 0";
            break;
        case 3 :
            viewPortTex = &gradientTexture01;
            modeName = "Gradient output 2";
            break;
        case 4 :
            viewPortTex = &derivationTexture;
            modeName = "Derivative output";
            break;
        case 5 :
            viewPortTex = &gradientTexture00;
            modeName = "Gradient output 1";
            break;
        case 6 :
            viewPortTex = &prepareLicTexture01;
            modeName = "prepare LIC output 1";
            break;
        case 7 :
            viewPortTex = &prepareLicTexture02;
            modeName = "prepare LIC output 2";
            break;
        case 8 :
            viewPortTex = &contourTexture;
            modeName = "Contour output";
            break;
        case 9 :
            viewPortTex = &ssboTexture;
            modeName = "SSBO Debug";
            break;
        default:
            viewPortTex = &tex;
            break;
        }

        ssboFBO.unbind();

        quadShader.bind();
        quadShader.uploadUniform(*viewPortTex, "tex");

        quad.draw();


        // GUI STUFF
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::Text(modeName.c_str());
            ImGui::SliderInt("Mode", &mode, 0, 9);

            ImGui::Checkbox("ViewSpace Distance", &viewSpaceDistance);
            ImGui::Checkbox("Perpendicular filtering", &stipplingLineConnection);
            ImGui::SliderFloat("viewSpaceMaxDistance", &viewSpaceMaxDistance, 0.0f, 1.0f);
            ImGui::SliderFloat("Contrast", &licContrast, 0.0f, 10.0f);
            ImGui::SliderFloat("LIC Angle", &licAngle, 0.0f, M_PI * 2);
            ImGui::SliderFloat("2nd LIC Angle (crosshatching)", &secondLicAngle, 0.0f, M_PI * 2);
            ImGui::SliderFloat("input_dotRad", &input_dotRad, 0.0f, 0.5f);
            ImGui::SliderFloat("Core Scale", &coreSize, 0.0f, 1.0f);
            ImGui::SliderFloat("wingRange", &wingRange, 0.0f, 1.0f);
            ImGui::SliderFloat("phongTransition", &phongTransition, 0.0f, 1.0f);
            ImGui::SliderFloat("smoothFactor", &smoothFactor, 0.0f, 10.0f);
            ImGui::SliderInt("snapGradientAmount", &snapGradientAmount, 0, 20);
            ImGui::SliderFloat3("PhongLightDirection", glm::value_ptr(phonglightDir), -1.0f, 1.0f);


            if(ImGui::CollapsingHeader("Contour"))
            {
                ImGui::SliderFloat("lineWidth", &lineWidth, 0.0f, 0.5f);
                ImGui::SliderFloat("lineFlatness", &lineFlatness, 0.0f, 1.0f);
                ImGui::SliderFloat("lineColor", &lineColor, 0.0f, 1.0f);
            }
            if(ImGui::CollapsingHeader("LIC Preparation"))
            {
                ImGui::SliderFloat("outerStrokeColor", &outerStrokeColor, 0.0f, 1.0f);
                ImGui::SliderFloat("wingRange", &wingRange, 0.0f, 1.0f);
                ImGui::SliderFloat("featureSize", &featureSize, 0.0f, 2.0f);
                ImGui::SliderFloat("radialDistanceReverseTransition", &radialDistanceReverseTransition, 0.0f, 1.0f);
                ImGui::SliderFloat("radial_distance_thresh", &radial_distance_thresh, 0.0f, 2.0f);
                ImGui::SliderFloat("permeabilityFact", &permeabilityFact, 0.0f, 8.0f);
                ImGui::SliderFloat("blendCurvatureFact", &blendCurvatureFact, 0.0f, 2.0f);
            }
            if(ImGui::CollapsingHeader("LIC"))
            {
                ImGui::SliderInt("LICIteration", &LICIteration, 0, 50);
                ImGui::SliderFloat("stepFactor", &stepFactor, 0.0f, 60.0f);
                ImGui::SliderFloat("lookUpDist", &lookUpDist, 1.0f, 30.0f);
            }

            ImGui::Text("Average %.3f ms/frame \n(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        }

        ImGui::Render();

        bool enableMouse = !io.WantCaptureMouse;

        win.enableMouseNavigation(enableMouse);

        // END OF FRAME
        win.swapBuffer();
    }

    return 0;
}
