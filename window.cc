#include "./window.h"
#include "./WindowCallbacks.h"
namespace Lina{ namespace Graphics{
    Window::Window(Window::specs& s) : mSpecs(s)
    {
       Create(s);
    }
    Window::~Window()
    {
        if (mWindow)
        {
            glfwDestroyWindow(mWindow);
        }
        glfwTerminate();
    }
    b8 Window::Create(specs &s)
    {
        if (!init())
        {
            return false;
            glfwTerminate();
        }
        return true;
    }
    b8 Window::init()
    {
        if (!glfwInit())
        {
            return false;
        }
        glfwWindowHint(GLFW_RESIZABLE, mSpecs.resizable);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
       mWindow = glfwCreateWindow(
               mSpecs.width,
               mSpecs.height,
               mSpecs.name,
               nullptr,
               nullptr
               );
        if (!mWindow)
        {
            glfwTerminate();
            std::cout<<"Error creating the Screen";
            return false;
        }
        glfwSetWindowUserPointer(mWindow, &mSpecs);
        glfwSetFramebufferSizeCallback(mWindow, Callbacks::WindowResize);
        glfwSwapInterval(4);
       return true;
    }
    b8 Window::isResized()
    {
        return mSpecs.resized;
    }
    b8 Window::isClosed()
    {
        return glfwWindowShouldClose(mWindow);
    }
    void Window::update()
    {
        glfwPollEvents();
        glfwGetFramebufferSize(mWindow, (i32*)&mSpecs.width, (i32*)&mSpecs.height);
        glfwSwapBuffers(mWindow);
    }
    b8 Window::createWindowSurface(VkInstance& instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, mWindow, nullptr, surface) == VK_SUCCESS)
            return true;
        return false;
    }
}}
