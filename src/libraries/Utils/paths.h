//
// Created by rheidrich on 02.03.17.
//

#ifndef EZR_PROJECT_PATHS_H
#define EZR_PROJECT_PATHS_H

#include "debug.h"

namespace ezr
{
    namespace paths
    {
        std::string loadFile(std::string path);
        std::string loadFileWithLog(std::string path);
    };
}


#endif //EZR_PROJECT_PATHS_H
