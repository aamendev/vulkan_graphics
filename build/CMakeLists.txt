cmake_minimum_required(VERSION 3.17)

project(main)
set(CMAKE_CXX_COMPILER g++)
set(GCCCompilerFlags "-std=c++23")
set(LDFLAGS "-ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi")
FILE(GLOB SRC
        "../*.h"
        "../*.cc"
        "../shapes/*.h"
        "../shapes/*.cc"
        "../Math/*.h"
        "../Math/src/*.cpp"
        "../third_party/*.h"
        "../third_party/*.cc"
        "../layers/*.h"
        "../layers/*.cc"
    )
add_executable(${PROJECT_NAME} ${SRC})
set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(${PROJECT_NAME} GL dl glfw vulkan)
