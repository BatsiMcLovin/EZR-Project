#include "assets.h"

const aiScene* ezr::assets::import(std::string path)
{
    //Importer that loads the scene from hard drive
    const aiScene* scene = importer.ReadFile( path,
                                              aiProcess_CalcTangentSpace       |
                                              aiProcess_GenNormals             |
                                              aiProcess_Triangulate            |
                                              aiProcess_JoinIdenticalVertices  |
                                              aiProcess_SortByPType            |
                                              aiProcess_ValidateDataStructure  |
                                              aiProcess_OptimizeMeshes          |
                                              aiProcess_PreTransformVertices      );


    //check if path and file was valid
    if(!scene)
    {
        ezr::debug::println("ERROR LOADING '" + path + "'" );
    } else
    {
        ezr::debug::println("SUCCESS LOADING '" + path + "'\n" + importer.GetErrorString());
    }
    return scene;
}

std::vector<ezr::Renderable *> ezr::assets::createRenderables(const aiScene* scene, std::string textureSearchPath)
{

    std::vector<Renderable *> renderables;
    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh * mesh = scene->mMeshes[i];
        aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
        renderables.push_back(new ezr::Renderable(mesh, material, textureSearchPath));
    }
    return renderables;
}

std::vector<ezr::Renderable *> ezr::assets::createRenderables(std::string path)
{
    return createRenderables(import(path), getPathFromString(path));
}

std::string ezr::assets::getPathFromString(std::string path)
{

    std::string directory = path.substr(path.find_last_of('/') + 1);
    std::string objName = directory;
    directory = path.substr(0, path.length() - directory.length());

    return directory;
}

const aiScene* ezr::assets::removeDoublesAndSmooth(const aiScene* scene)
{
    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        scene->mMeshes[i] = removeDoublesAndSmooth(scene->mMeshes[i]);
    }

    return scene;
}

aiMesh *ezr::assets::removeDoublesAndSmooth(aiMesh* mesh)
{
    std::vector<aiFace> faces(mesh->mFaces, mesh->mFaces + mesh->mNumFaces);

    std::map<aiVector3D, GLuint> posMap;

    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        faces[i].mNumIndices = 3;
        for (int j = 0; j < 3; ++j)
        {
            glm::uint index = face.mIndices[j];
            aiVector3D position = mesh->mVertices[index];

            if(posMap.find(position) != posMap.end())
            {
                index = posMap[position];
            }
            else
            {
                posMap[position] = index;
            }

            faces[i].mIndices[j] = index;
        }

    }


    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            mesh->mFaces[i].mIndices[j] = faces[i].mIndices[j];
        }
    }

    return mesh;

}
