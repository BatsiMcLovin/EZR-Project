//
// Created by dino on 16.11.15.
//

#ifndef EZR_PROJECT_RENDERABLE_H
#define EZR_PROJECT_RENDERABLE_H

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Utils/debug.h"

#include <FBO/Texture.h>



namespace ezr
{
    enum class Matkey_v3
    {
        DIFFUSE_COLOR,
        SPECULAR_COLOR,
        AMBIENT_COLOR,
        EMISSIVE_COLOR,
        REFLECTIVE_COLOR,
        TRANSPARENT_COLOR
    };

    enum class Matkey_v1
    {
        SPECULAR_HARDNESS,
        BUMP_SCALING,
        REFLECTIVITY,
        SHININESS
    };

    enum class Matkey_tex2D
    {
        DIFFUSE_TEXTURE,
        NORMAL_TEXTURE,
        AMBIENT_TEXTURE,
        SPECULAR_TEXTURE,
        SHININESS_TEXTURE,
        EMISSIVE_TEXTURE
    };

    class Renderable
    {
    public:
        Renderable();

        Renderable(aiMesh* mesh, aiMaterial* material, std::string textureSearchPath = "");

        //Decomposal Kram für Mesh und Material (aus Assimp Daten für Shader rausholen)
        void decompose(aiMesh* mesh);
        void decompose(aiMaterial* material, std::string textureSearchPath = "");

        // VBO Creation
        GLuint createVbo(aiVector3D * vert, GLuint num,  GLuint dimensions, GLuint vertexAttributePointer);
        GLuint createVbo(std::vector<float> vert, GLuint num,  GLuint dimensions, GLuint vertexAttributePointer);
        GLuint createIndexVbo(std::vector<unsigned int> content);

        //Attribute Setter
        void setAttribute(Matkey_v3 key, glm::vec3 vec);
        void setAttribute(Matkey_v1 key, float val);
        void setAttribute(Matkey_tex2D key, ezr::Texture texture);

        //Attribute Getter
        glm::vec3 getAttribute(Matkey_v3 key);
        float getAttribute(Matkey_v1 key);
        ezr::Texture getAttribute(Matkey_tex2D key);

        //Has Attribute
        bool hasAttribute(Matkey_v3 key);
        bool hasAttribute(Matkey_v1 key);
        bool hasAttribute(Matkey_tex2D key);

        //OpenGL befehle

        void bind();
        void unbind();

        virtual void draw();
        virtual void draw(GLenum mode);

        GLuint getIndexCount();

        GLuint getVertexCount();

        GLint getVAO();

		glm::mat4 getModelMatrix();

		glm::vec3 _position = glm::vec3(0.0f);
		glm::vec3 _rotation = glm::vec3(0.0f);;
		glm::vec3 _scale = glm::vec3(1.0f);;

        void free();

    protected:
        GLenum _mode;

        GLuint _vao;

        GLuint _indexCount;
        GLuint _vertexCount;

        std::vector<GLuint> _vboHandles;

        std::map<Matkey_v3, glm::vec3> _vec3Map;
        std::map<Matkey_v1, float> _vec1Map;
        std::map<ezr::Matkey_tex2D, ezr::Texture> _vecTex2DMap;
    };
}

#endif //EZR_PROJECT_RENDERABLE_H
