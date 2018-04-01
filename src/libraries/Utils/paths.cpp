//
// Created by rheidrich on 02.03.17.
//

#include "paths.h"

std::string ezr::paths::loadFile(std::string path)
{
    std::ifstream file;
    file.open(path.c_str(), std::ios_base::in);

    if(!file)
        return "";

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}

std::string ezr::paths::loadFileWithLog(std::string path)
{
    ezr::debug::println("Tying to open \'" + path + "\'");
    return ezr::paths::loadFile(path);
}