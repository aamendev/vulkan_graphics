#include "WindowCallbacks.h"
#include "window.h"
namespace Lina{ namespace Callbacks{
    void WindowResize(GLFWwindow* window, int width, int height)
    {
       auto pWindow = reinterpret_cast<Graphics::Window::specs*>
           (glfwGetWindowUserPointer(window));
       pWindow->resized = true;
    }
}}
