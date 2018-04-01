cmake_minimum_required(VERSION 2.8)
project(EZR_project)

## Verwende C++ 11 in diesem Projekt
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
add_compile_options(-std=c++11)

## Der Module Path ist wo noch mehr CMake Kram drin ist
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(EZR_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Project specific path. Set manually if it was not found.")

find_package(OpenGL REQUIRED)

## Examples externer Libraries muessen nicht mitgebuildet werden
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")

## Die externen Libraries werden hier als Unterverzeichnis hinzugefügt,
## sodass deren CMakeList.txt ausgeführt wird.
add_subdirectory(${PROJECT_SOURCE_DIR}/externals/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/externals/glew)
add_subdirectory(${PROJECT_SOURCE_DIR}/externals/assimp)

include_directories(${PROJECT_SOURCE_DIR}/externals/glm)

## Alle include-Verzeichnisse, die der gnu-Compiler noch nicht von alleine kennt
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/externals/assimp/include)
include_directories(${PROJECT_SOURCE_DIR}/externals/glew/include)

##Unser lib-Ordner muss auch erst als bekannt gegeben werden
include_directories(src/libraries)

##Heißt nichts anderes als, dass rekursiv nach Source und Headern geguckt wird im lib-Ordner
## SRC und HEAD sind ab jetzt Abkürzung für die Pfade aller .cpp und .h Dateien
FILE(GLOB_RECURSE EZR_SRC ${PROJECT_SOURCE_DIR}/src/libraries/*.cpp )
FILE(GLOB_RECURSE EZR_HEAD ${PROJECT_SOURCE_DIR}/src/libraries/*.h)
set(EZR_SRC ${EZR_SRC} CACHE PATH "EZR Source Path.")
set(EZR_HEAD ${EZR_HEAD} CACHE PATH "EZR Header Path.")

##Vereine alle Dependencies zu einer Variable zusammen
set(ALL_LIBS assimp libglew_shared glfw ${OPENGL_LIBRARIES} CACHE STRING "All ezr dependency libs.")

## Die Option hinterfragt ob die ezr demos gebuildet werden müssen
option(EZR_BUILD_DEMOS "Build EZR demo programs" ON)


if(EZR_BUILD_DEMOS)

    ## Hier werden alle executables deklariert (also jene die eine main haben)
    ## erstes argument ist der cmake-variablenname (zB x, y, peter) und das zweite der pfad zur cpp-Datei
    ## alles was danach kommt, sind die Src und Header files aus unserem Project
    add_executable(demo_assetloader src/executables/demo_assetloader.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_toon src/executables/demo_toon.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_antialiasing src/executables/demo_antialiasing.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_shadowmapping src/executables/demo_shadowmapping.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_lighting src/executables/demo_lighting.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_ambientocclusion src/executables/demo_ambient_occlusion.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_medcv src/executables/demo_medcv.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_ssbo src/executables/demo_ssbo.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_sloan src/executables/demo_sloan.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_computeshader src/executables/demo_computeshader.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_cubemap src/executables/demo_cubemap.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_memoryallocation src/executables/demo_memoryallocation.cpp ${EZR_SRC} ${EZR_HEAD})
    add_executable(demo_atomiccounter src/executables/demo_atomiccounter.cpp ${EZR_SRC} ${EZR_HEAD})


    ## Alle externen Libraries werden "gegen" die executable gelinkt
    ## im Klartext: diese Executable braucht Library XY und Z
    target_link_libraries(demo_assetloader          ${ALL_LIBS})
    target_link_libraries(demo_toon                 ${ALL_LIBS})
    target_link_libraries(demo_antialiasing         ${ALL_LIBS})
    target_link_libraries(demo_shadowmapping        ${ALL_LIBS})
    target_link_libraries(demo_lighting             ${ALL_LIBS})
    target_link_libraries(demo_ambientocclusion     ${ALL_LIBS})
    target_link_libraries(demo_medcv                ${ALL_LIBS})
    target_link_libraries(demo_ssbo                 ${ALL_LIBS})
    target_link_libraries(demo_sloan                ${ALL_LIBS})
    target_link_libraries(demo_computeshader        ${ALL_LIBS})
    target_link_libraries(demo_cubemap              ${ALL_LIBS})
    target_link_libraries(demo_memoryallocation     ${ALL_LIBS})
    target_link_libraries(demo_atomiccounter        ${ALL_LIBS})

endif()

## Hier sage ich dass der PROJECT_PATH als macro in unserem Code bekannt sein soll
set(EZR_PROJECT_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Project specific path. Set manually if it was not found.")
add_definitions(-DEZR_PROJECT_PATH="${EZR_PROJECT_PATH}")

## Hier sage ich dass der PROJECT_PATH als macro in unserem Code bekannt sein soll
set(EZR_SHADER_PATH ${PROJECT_SOURCE_DIR}/src/shaders CACHE PATH "Project specific path. Set manually if it was not found.")
add_definitions(-DEZR_SHADER_PATH="${EZR_SHADER_PATH}")

