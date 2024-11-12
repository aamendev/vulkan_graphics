#ifndef WINDOW_CALLBACK_H
#define WINDOW_CALLBACK_H
#include "./events/all_events.h"
#include "./window.h"
namespace Lina{ namespace Callbacks{
    void Key(GLFWwindow* window, int key, int scancode, int action, int modes);
    void MouseButton(GLFWwindow* window, int button, int action, int mod);
}}
#endif
