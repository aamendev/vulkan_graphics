#ifndef WINDOW_EVENT_H
#define WINDOW_EVENT_H
#include "events.h"

namespace Lina{ namespace Events{
    class WindowResize : public Event
    {
        public:
            WindowResize(i32 width, i32 height) : mWidth(width), mHeight(height){}
            i32 getWidth() const {return mWidth;}
            i32 getHeight() const {return mHeight;}
            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::WindowResize;}
        private:
            i32 mWidth;
            i32 mHeight;
    };
    class WindowClose : public Event
    {
        public:
            WindowClose() = default;
    };
}}
#endif
