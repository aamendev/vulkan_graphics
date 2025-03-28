#include "callbacks.h"
#include "types.h"
#include "events/mouse.h"
namespace Lina{ namespace Callbacks{
    void Key(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Graphics::Window::specs& p = 
            *reinterpret_cast<Graphics::Window::specs*>(glfwGetWindowUserPointer(window));
        switch (action)
        {
            case GLFW_PRESS:
                {
                    auto* event = new Events::KeyPress((Input::KeyCode)key, false);
                    p.mEvents.push_back(event);
                    break;
                }
            case GLFW_REPEAT:
                {
                    auto* event = new Events::KeyPress((Input::KeyCode)key, true);
                    p.mEvents.push_back(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    auto* event = new Events::KeyRelease((Input::KeyCode)key);
                    p.mEvents.push_back(event);
                    break;
                }
        }
    }

    void MouseButton(GLFWwindow* window, int button, int action, int mod)
    {
        Graphics::Window::specs& p = 
            *reinterpret_cast<Graphics::Window::specs*>(glfwGetWindowUserPointer(window));
        switch(action)
        {
            case GLFW_PRESS:
                {
                    auto* event = new Events::MouseButtonPressed((Input::MouseCode)button);
                    p.mEvents.push_back(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    auto* event = new Events::MouseButtonReleased((Input::MouseCode)button);
                    p.mEvents.push_back(event);
                    break;
                }
        }
    }

    void MouseMove(GLFWwindow* window, f64 xpos, f64 ypos)
    {
        Graphics::Window::specs& p = 
            *reinterpret_cast<Graphics::Window::specs*>(glfwGetWindowUserPointer(window));
       auto* event = new Events::MouseMove(xpos, ypos);
       p.mEvents.push_back(event);
    }
    void WindowResize(GLFWwindow* window, int width, int height)
    {
       auto pWindow = reinterpret_cast<Graphics::Window::specs*>
           (glfwGetWindowUserPointer(window));
       auto* event = new Events::WindowResize(width, height);
       pWindow->mEvents.push_back(event);
       pWindow->width = width;
       pWindow->height = height;
       pWindow->resized = true;
    }
}}
