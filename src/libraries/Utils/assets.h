//
// Created by dino on 03.11.15.
//

#ifndef EZR_PROJECT_ASSETS_H
#define EZR_PROJECT_ASSETS_H

#include "debug.h"

#include "Renderables/Renderable.h"

namespace ezr
{

    namespace assets
    {
        const aiScene* import(std::string path);
        std::vector<ezr::Renderable*> createRenderables(const aiScene* scene, std::string textureSearchPath = "");
        std::vector<ezr::Renderable*> createRenderables(std::string path);

        std::string getPathFromString(std::string path);

        const aiScene* removeDoublesAndSmooth(const aiScene* scene);
        aiMesh* removeDoublesAndSmooth(aiMesh* mesh);

        static Assimp::Importer importer;
    }

}


#endif //EZR_PROJECT_ASSETS_H
