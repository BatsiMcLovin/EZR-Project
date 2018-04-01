//
// Created by dino on 16.11.15.
//

#include "Renderable.h"

ezr::Renderable::Renderable(aiMesh *mesh, aiMaterial *material, std::string textureSearchPath)
{
    decompose(mesh);
    decompose(material, textureSearchPath);

    _mode = GL_TRIANGLES;
}

ezr::Renderable::Renderable()
{

}


void ezr::Renderable::decompose(aiMesh *mesh)
{
    //Erstmal vao Handle bauen

    glGenVertexArrays(1, &_vao);

    // binden

    bind();

    //Indizes aus Assimp rausholen
    std::vector<unsigned int> indices;
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        //Alle Indizes pro Face (Triangle, der Assetloader trianguliert...)
        for (int j = 0; j < mesh->mFaces->mNumIndices; ++j)
        {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    // #indices = indexCount
    _vertexCount = mesh->mNumVertices;


    // Index VBO
    if (indices.size() != 0)
        _vboHandles.push_back(createIndexVbo(indices));


    // Vertex VBO
    if (mesh->mNumVertices > 0)
        _vboHandles.push_back(createVbo(mesh->mVertices, mesh->mNumVertices, 3, 0));

    // UV VBO
    if (mesh->HasTextureCoords(0))
    {
        std::vector<float> texCoords;
        for (unsigned int k = 0; k < mesh->mNumVertices; ++k)
        {
            texCoords.push_back(mesh->mTextureCoords[0][k].x);
            texCoords.push_back(mesh->mTextureCoords[0][k].y);
        }
        _vboHandles.push_back(createVbo(texCoords, texCoords.size(), 2, 1));
    }

    // Normal/ Tangent/ Bitangent VBO
    if (mesh->mNumVertices > 0)
        _vboHandles.push_back(createVbo(mesh->mNormals, mesh->mNumVertices, 3, 2));
    if (mesh->HasTangentsAndBitangents())
    {
        _vboHandles.push_back(createVbo(mesh->mTangents, mesh->mNumVertices, 3, 3));
        _vboHandles.push_back(createVbo(mesh->mBitangents, mesh->mNumVertices, 3, 4));
    }

    // entbinden
    unbind();


}

void ezr::Renderable::decompose(aiMaterial *material, std::string textureSearchPath)
{
    aiColor4D temp;
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &temp))
        setAttribute(Matkey_v3::DIFFUSE_COLOR, glm::vec3(temp.r, temp.g, temp.b));
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &temp))
        setAttribute(Matkey_v3::AMBIENT_COLOR, glm::vec3(temp.r, temp.g, temp.b));
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &temp))
        setAttribute(Matkey_v3::SPECULAR_COLOR, glm::vec3(temp.r, temp.g, temp.b));
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &temp))
        setAttribute(Matkey_v3::EMISSIVE_COLOR, glm::vec3(temp.r, temp.g, temp.b));
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_REFLECTIVE, &temp))
        setAttribute(Matkey_v3::REFLECTIVE_COLOR, glm::vec3(temp.r, temp.g, temp.b));
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_TRANSPARENT, &temp))
        setAttribute(Matkey_v3::TRANSPARENT_COLOR, glm::vec3(temp.r, temp.g, temp.b));

    float temp00;
    if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &temp00))
        setAttribute(Matkey_v1::SPECULAR_HARDNESS, temp00);
    if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_BUMPSCALING, &temp00))
        setAttribute(Matkey_v1::BUMP_SCALING, temp00);
    if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_REFLECTIVITY, &temp00))
        setAttribute(Matkey_v1::REFLECTIVITY, temp00);
    if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &temp00))
        setAttribute(Matkey_v1::SHININESS, temp00);

    aiString temp01;
    if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &temp01))
        setAttribute(Matkey_tex2D::DIFFUSE_TEXTURE, ezr::Texture(textureSearchPath + temp01.data));
    if (AI_SUCCESS == material->GetTexture(aiTextureType_NORMALS, 0, &temp01))
        setAttribute(Matkey_tex2D::NORMAL_TEXTURE, ezr::Texture(textureSearchPath + temp01.data));
    if (AI_SUCCESS == material->GetTexture(aiTextureType_AMBIENT, 0, &temp01))
        setAttribute(Matkey_tex2D::AMBIENT_TEXTURE, ezr::Texture(textureSearchPath + temp01.data));
    if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &temp01))
        setAttribute(Matkey_tex2D::SPECULAR_TEXTURE, ezr::Texture(textureSearchPath + temp01.data));
    if (AI_SUCCESS == material->GetTexture(aiTextureType_SHININESS, 0, &temp01))
        setAttribute(Matkey_tex2D::SHININESS_TEXTURE, ezr::Texture(textureSearchPath + temp01.data));
    if (AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &temp01))
        setAttribute(Matkey_tex2D::EMISSIVE_TEXTURE, ezr::Texture(textureSearchPath + temp01.data));
}

GLuint ezr::Renderable::createVbo(aiVector3D *vert, GLuint num, GLuint dimensions, GLuint vertexAttributePointer)
{
    GLuint vbo = 0;

    if (num != 0)// && content.size() % dimensions == 0 )
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, num * sizeof(aiVector3D), vert, GL_STATIC_DRAW);
        glVertexAttribPointer(vertexAttributePointer, dimensions, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(vertexAttributePointer);
    }

    return vbo;
}

GLuint ezr::Renderable::createVbo(std::vector<float> vert, GLuint num, GLuint dimensions,
                                  GLuint vertexAttributePointer)
{
    GLuint vbo = 0;

    if (num != 0)// && content.size() % dimensions == 0 )
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);
        glVertexAttribPointer(vertexAttributePointer, dimensions, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(vertexAttributePointer);
    }

    return vbo;
}

GLuint ezr::Renderable::createIndexVbo(std::vector<unsigned int> content)
{

    GLuint vbo = 0;
    _indexCount = content.size();

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(unsigned int), &content[0], GL_STATIC_DRAW);


    return vbo;
}

void ezr::Renderable::bind()
{
    glBindVertexArray(_vao);
}

void ezr::Renderable::unbind()
{
    glBindVertexArray(0);
}

void ezr::Renderable::draw()
{
    draw(_mode);
}

void ezr::Renderable::draw(GLenum mode)
{
    bind();

    glDrawElements(mode, _indexCount, GL_UNSIGNED_INT, 0);

    unbind();
}

GLuint ezr::Renderable::getIndexCount()
{
    return _indexCount;
}

GLuint ezr::Renderable::getVertexCount()
{
    return _vertexCount;
}

void ezr::Renderable::setAttribute(ezr::Matkey_v3 key, glm::vec3 vec)
{
    _vec3Map.insert(std::pair<ezr::Matkey_v3, glm::vec3>(key, vec));
}

void ezr::Renderable::setAttribute(ezr::Matkey_v1 key, float val)
{
    _vec1Map.insert(std::pair<ezr::Matkey_v1, float>(key, val));
}

void ezr::Renderable::setAttribute(ezr::Matkey_tex2D key, ezr::Texture texture)
{
    _vecTex2DMap.insert(std::pair<ezr::Matkey_tex2D, ezr::Texture>(key, texture));
}

glm::vec3 ezr::Renderable::getAttribute(ezr::Matkey_v3 key)
{
    if (hasAttribute(key))
        return _vec3Map[key];
    else
        return glm::vec3(0.0f);
}

float ezr::Renderable::getAttribute(ezr::Matkey_v1 key)
{
    if (hasAttribute(key))
        return _vec1Map.at(key);
    else
        return 0.0f;
}

ezr::Texture ezr::Renderable::getAttribute(ezr::Matkey_tex2D key)
{
    if (hasAttribute(key))
        return _vecTex2DMap.at(key);
    else
        return ezr::Texture(0, 0, 0);
}

bool ezr::Renderable::hasAttribute(ezr::Matkey_v3 key)
{
    std::map<ezr::Matkey_v3, glm::vec3>::iterator iter = _vec3Map.find(key);

    return iter != _vec3Map.end();
}

bool ezr::Renderable::hasAttribute(ezr::Matkey_v1 key)
{
    std::map<ezr::Matkey_v1, float>::iterator iter = _vec1Map.find(key);

    return iter != _vec1Map.end();
}

bool ezr::Renderable::hasAttribute(ezr::Matkey_tex2D key)
{
    std::map<ezr::Matkey_tex2D, ezr::Texture>::iterator iter = _vecTex2DMap.find(key);

    return iter != _vecTex2DMap.end();
}

GLint ezr::Renderable::getVAO()
{
    return _vao;
}

glm::mat4 ezr::Renderable::getModelMatrix()
{
	return glm::translate(_position) * glm::scale(_scale);
}

void ezr::Renderable::free()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers((int)_vboHandles.size(), &_vboHandles[0]);
}

