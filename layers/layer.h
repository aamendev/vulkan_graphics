#ifndef LAYER_H
#define LAYER_H
#include "../events/key.h"
#include "../events/mouse.h"
namespace Lina { namespace Graphics {
class Layer
{
    public:
        virtual ~Layer() = default;
        virtual void run() = 0;
        virtual void init() = 0;
        virtual void onKeyDown(Events::KeyPress& e) = 0;
        virtual void onMouseMove(Events::MouseMove& m) = 0;
        virtual void onKeyUp(Events::KeyRelease& e) = 0;
};
}}
#endif
