#pragma once
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;
typedef long double f128;

typedef bool b8;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presnetModes;
};
struct QueueFamilyIndices
{
    i32 graphicsFamily;
    i32 presentFamily;

    bool isComplete()
    {
        return graphicsFamily > -1  && presentFamily > - 1;
    }
};


enum class Primitive
{
    Triangle = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    LineStrip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
};
