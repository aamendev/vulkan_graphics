cmake_minimum_required(VERSION 3.17)

project(main)

find_package(Vulkan REQUIRED)
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
        "../ecs/*.h"
        "../ecs/*.cc"
        "../ecs/systems/*.h"
        "../ecs/systems/*.cc"
        "../ecs/components/colliders/*.h"
        "../ecs/components/colliders/*.cc"
        "../ecs/components/*.h"
        "../ecs/components/*.cc"
        "../game_system/*.h"
        "../game_system/*.cc"
        "../core/*.h"
        "../core/*.cc"
        "../physics/*.h"
        "../physics/*.cc"
        "../games/*.h"
        "../games/*.cc"
    )
add_executable(${PROJECT_NAME} ${SRC})
set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(${PROJECT_NAME} GL dl glfw vulkan)
