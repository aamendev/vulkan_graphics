#ifndef KEY_EVENT_H
#define KEY_EVENT_H
#include "events.h"
namespace Lina{ namespace Events{
    class KeyPress : public Event
    {
        public:
            KeyPress(Input::KeyCode keycode, b8 isHold = false): 
                mkeyCode(keycode), mIsHold(isHold){}
            b8 isHold() const {return mIsHold;}
            Input::KeyCode key() const {return mkeyCode;}
            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::KeyDown;}
        private:
            Input::KeyCode mkeyCode;
            b8 mIsHold;
    };

    class KeyRelease : public Event
    {
        public:
            KeyRelease(Input::KeyCode keycode): 
                mkeyCode(keycode){}
            Input::KeyCode key() const {return mkeyCode;}

            virtual Type getType() const override {return getStaticType();}
            static Type getStaticType() {return Type::KeyUp;}
        private:
            Input::KeyCode mkeyCode;
    };
}}
#endif
