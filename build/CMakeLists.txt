cmake_minimum_required(VERSION 3.17)

project(main)
set(CMAKE_CPP_COMPILER clang++)
SET(GCCCompilerFlags "-std=c++23")
SET(LDFLAGS "-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi")
#SET(GCCLinkerFlags    "--no-as-needed")
#SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCCCompilerFlags}")
#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCCLinkerFlags}")
FILE(GLOB SRC
        "../*.h"
        "../*.cc"
        "../Math/*.h"
        "../Math/src/*.cpp"
        "../third_party/*.h"
        "../third_party/*.cc"
    )
add_executable(${PROJECT_NAME} ${SRC})
set (CMAKE_BUILD_TYPE Debug)
find_package(glm CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} glm)
target_link_libraries(${PROJECT_NAME} GL dl glfw)
target_link_libraries(${PROJECT_NAME} vulkan)
