#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H
#include "events.h"

namespace Lina{ namespace Events{
    class MouseMove : public Event
    {
        public:
            MouseMove(f32 x, f32 y): mX(x), mY(y){}
        public:
            float x() const {return mX;}
            float y() const {return mY;}
            std::pair<float, float> get() const {return {mX, mY};}
            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::MouseMove;}
        private:
            f32 mX;
            f32 mY;
    };

    class MouseScroll : public Event
    {
        public:
            MouseScroll(f32 x, f32 y): mX(x), mY(y){}
        public:
            float x() const {return mX;}
            float y() const {return mY;}
            std::pair<float, float> get() const {return {mX, mY};}
            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::MouseScroll;}
        private:
            f32 mX;
            f32 mY;
    };

    class MouseButtonPressed : public Event
    {
        public:
            MouseButtonPressed(Input::MouseCode button): mButton(button){}
            Input::MouseCode button() const {return mButton;}
            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::MouseDown;}
        private:
            Input::MouseCode mButton;
    };

    class MouseButtonReleased : public Event
    {
        public:
            MouseButtonReleased(Input::MouseCode button): mButton(button){}
            Input::MouseCode button() const {return mButton;}
            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::MouseUp;}
        private:
            Input::MouseCode mButton;
    };
}}
#endif
