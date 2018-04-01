//
// Created by rheidrich on 03.03.17.
//

#include "ShaderUtils.h"


const std::string shaderStart = "#version 330\n"
        "\n"
        "in vec3 passPosition_w;\n"
        "in vec2 passUV;\n"
        "in vec3 passNormal_w;\n"
        "in vec3 passTangent_w;\n"
        "in vec3 passBitangent_w;\n"
        "\n";

const std::string outputs = "\n"
        "layout(location = 0) out vec4 diffuseOutput;\n"
        "layout(location = 1) out vec4 specularOutput;\n"
        "layout(location = 2) out vec4 emissiveOutput;\n"
        "layout(location = 3) out vec4 normalOutput;\n"
        "layout(location = 4) out vec4 positionOutput;\n"
        "layout(location = 5) out vec4 materialIDOutput;\n\n";

const std::string transformToTangentSpace = "vec3 transformToTangentSpace(vec3 vec)\n"
        "{\n"
        "    vec3 bitangent = cross(passNormal_w, passTangent_w);\n"
        "    mat3 matrix = mat3(passTangent_w, bitangent, passNormal_w);\n"
        "\n"
        "    return matrix * vec;\n"
        "}\n\n";


ezr::SimpleShader ezr::ShaderUtils::loadFromPaths(std::string vertexShaderPath, std::string fragmentShaderPath)
{
    return SimpleShader({{ezr::EZR_VERTEX_SHADER_PATH,   vertexShaderPath},
                         {ezr::EZR_FRAGMENT_SHADER_PATH, fragmentShaderPath}
                        });
}

ezr::SimpleShader ezr::ShaderUtils::loadFromPaths(std::string vertexShaderPath,
                                                  std::string geometryShaderPath,
                                                  std::string fragmentShaderPath)
{
    return SimpleShader({{ezr::EZR_VERTEX_SHADER_PATH,   vertexShaderPath},
                         {ezr::EZR_GEOMETRY_SHADER_PATH, geometryShaderPath},
                         {ezr::EZR_FRAGMENT_SHADER_PATH, fragmentShaderPath}
                        });
}

ezr::SimpleShader ezr::ShaderUtils::loadFromCode(std::string vertexShaderCode, std::string fragmentShaderCode)
{
    return SimpleShader({{ezr::EZR_VERTEX_SHADER_CODE,   vertexShaderCode},
                         {ezr::EZR_FRAGMENT_SHADER_CODE, fragmentShaderCode}
                        });
}

ezr::SimpleShader ezr::ShaderUtils::loadFromCode(std::string vertexShaderCode,
                                                 std::string geometryShaderCode,
                                                 std::string fragmentShaderCode)
{
    return SimpleShader({{ezr::EZR_VERTEX_SHADER_CODE,   vertexShaderCode},
                         {ezr::EZR_GEOMETRY_SHADER_CODE, geometryShaderCode},
                         {ezr::EZR_FRAGMENT_SHADER_CODE, fragmentShaderCode}
                        });
}

std::vector<ezr::SimpleShader> ezr::ShaderUtils::createGBuffers(std::vector<ezr::Renderable*> renderables)
{
    std::vector<ezr::SimpleShader> gbuffers;

    for(auto rend : renderables)
    {
        gbuffers.push_back(createGBuffer(*rend));
    }

    return gbuffers;
}

ezr::SimpleShader ezr::ShaderUtils::createGBuffer(ezr::Renderable renderable)
{
    std::string uniforms = "uniform int materialID = 0;\n";
    std::string assignments = "";
    std::string functions = "";

    // DIFFUSE
    if (renderable.hasAttribute(ezr::Matkey_tex2D::DIFFUSE_TEXTURE))
    {
        uniforms += "uniform sampler2D diffuseTexture;\n";
        assignments += "diffuseOutput = texture(diffuseTexture, passUV); \n";
    }
    else
    {
        if (renderable.hasAttribute(ezr::Matkey_v3::DIFFUSE_COLOR))
        {
            uniforms += "uniform ";
        }
        uniforms += "vec3 diffuseColor = vec3(0.1f, 0.1f, 0.5f);\n";
        assignments += "diffuseOutput = vec4(diffuseColor, 1); \n";
    }

    // SHININESS
    if (renderable.hasAttribute(ezr::Matkey_tex2D::SHININESS_TEXTURE))
    {
        uniforms += "uniform sampler2D shininessTexture;\n";
        assignments += "float shininessColor = texture(shininessTexture, passUV).x; \n";
    }
    else
    {
        if (renderable.hasAttribute(ezr::Matkey_v1::SHININESS))
        {
            uniforms += "uniform ";
        }
        uniforms += "float shininessColor = 0.5f;\n";
    }

    // SPECULAR
    if (renderable.hasAttribute(ezr::Matkey_tex2D::SPECULAR_TEXTURE))
    {
        uniforms += "uniform sampler2D specularTexture;\n";
        assignments += "specularOutput = vec4(texture(specularTexture, passUV).xyz, shininessColor); \n";
    }
    else
    {
        if (renderable.hasAttribute(ezr::Matkey_v3::SPECULAR_COLOR))
        {
            uniforms += "uniform ";
        }
        uniforms += "vec3 specularColor = vec3(1.0f);\n";
        assignments += "specularOutput = vec4(specularColor, shininessColor); \n";
    }

    // EMISSIVE
    if (renderable.hasAttribute(ezr::Matkey_tex2D::EMISSIVE_TEXTURE))
    {
        uniforms += "uniform sampler2D emissiveTexture;\n";
        assignments += "emissiveOutput = texture(emissiveTexture, passUV); \n";
    }
    else
    {
        if (renderable.hasAttribute(ezr::Matkey_v3::EMISSIVE_COLOR))
        {
            uniforms += "uniform ";
        }
        uniforms += "vec3 emissiveColor = vec3(0.0f);\n";
        assignments += "emissiveOutput = vec4(emissiveColor, 0.0f); \n";
    }

    // NORMALS
    if (renderable.hasAttribute(ezr::Matkey_tex2D::NORMAL_TEXTURE))
    {
        uniforms += "uniform sampler2D normalTexture;\n";
        assignments += "normalOutput = vec4(normalize(transformToTangentSpace(texture(normalTexture, passUV).xyz * 2 - 1)), 1);\n";
        functions += transformToTangentSpace;
    }
    else
    {
        assignments += "normalOutput = vec4(normalize(passNormal_w), 1);\n";
    }

    // POSITIONS
    assignments += "positionOutput = vec4(passPosition_w, 1);\n"
            "materialIDOutput = vec4(materialID);\n";

    //COMPOSE
    std::string fragmentShader = shaderStart +
                                 uniforms +
                                 outputs +
                                 functions +
                                 "\nvoid main() \n{\n\n" +
                                 assignments +
                                 "\n}";

    static std::map<std::string, ezr::SimpleShader> gbufferMap;

    if(gbufferMap.find(fragmentShader) == gbufferMap.end())
    {
        gbufferMap[fragmentShader] = loadFromCode(ezr::paths::loadFile(EZR_SHADER_PATH "/GBuffer/simpleTransform.vert"), fragmentShader);
    }

    return gbufferMap[fragmentShader];
}

std::map<ezr::GBufferOutput, ezr::Texture> ezr::ShaderUtils::createGBufferFBOTextures(ezr::FBO *fbo)
{
    std::map<ezr::GBufferOutput, ezr::Texture> fboTextures;

    fboTextures[DIFFUSE_OUTPUT] = fbo->generateColorBuffer("diffuseOutput", 0);
    fboTextures[SPECULAR_OUTPUT] = fbo->generateColorBuffer("specularOutput", 1);
    fboTextures[EMISSION_OUTPUT] = fbo->generateColorBuffer("emissiveOutput", 2);
    fboTextures[NORMALS_OUTPUT] = fbo->generateColorBuffer("normalOutput", 3);
    fboTextures[POSITION_OUTPUT] = fbo->generateColorBuffer("positionOutput", 4);
    fboTextures[MATERIAL_ID_OUTPUT] = fbo->generateColorBuffer("materialIDOutput", 5);
    fboTextures[DEPTH_OUTPUT] = fbo->generateDepthBuffer("depth");

    return fboTextures;
}

void ezr::ShaderUtils::uploadGBufferMaterialUniforms(ezr::SimpleShader shader, ezr::Renderable renderable)
{
    GLuint texIndex = 0;

    if (renderable.hasAttribute(Matkey_tex2D::DIFFUSE_TEXTURE)) //DIFFUSE //-------
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_tex2D::DIFFUSE_TEXTURE), "diffuseTexture", texIndex);
        texIndex++;
    }
    else
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_v3::DIFFUSE_COLOR), "diffuseColor");
    }
    if (renderable.hasAttribute(Matkey_tex2D::SPECULAR_TEXTURE)) //SPECULAR //-------
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_tex2D::SPECULAR_TEXTURE), "specularTexture", texIndex);
        texIndex++;
    }
    else
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_v3::SPECULAR_COLOR), "specularColor");
    }
    if (renderable.hasAttribute(Matkey_tex2D::EMISSIVE_TEXTURE)) //EMISSIVE //-------
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_tex2D::EMISSIVE_TEXTURE), "emissiveTexture", texIndex);
        texIndex++;
    }
    else
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_v3::EMISSIVE_COLOR), "emissiveColor");
    }
    if (renderable.hasAttribute(Matkey_tex2D::SHININESS_TEXTURE)) //SHININESS //-------
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_tex2D::SHININESS_TEXTURE), "shininessTexture", texIndex);
        texIndex++;
    }
    else
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_v1::SHININESS), "shininessColor");
    }

    if (renderable.hasAttribute(Matkey_tex2D::NORMAL_TEXTURE)) //NORMALS //-------
    {
        shader.uploadUniform(renderable.getAttribute(Matkey_tex2D::NORMAL_TEXTURE), "normalTexture", texIndex);
    }

    shader.uploadUniform(renderable.getVAO(), "materialID");
}

ezr::ComputeShader ezr::ShaderUtils::loadComputeShaderFromPath(std::string path)
{
    return ezr::ComputeShader(ezr::paths::loadFile(path));
}
