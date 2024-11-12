#ifndef WINDOW_EVENT_H
#define WINDOW_EVENT_H
#include "events.h"

namespace Lina{ namespace Events{
    class WindowResize : public Event
    {
        public:
            WindowResize(u32 width, u32 height) : mWidth(width), mHeight(height){}
            u32 getWidth() const {return mWidth;}
            u32 getHeight() const {return mHeight;}
        private:
            u32 mWidth;
            u32 mHeight;

    };
    class WindowClose : public Event
    {
        public:
            WindowClose() = default;
    };
}}
#endif
