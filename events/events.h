#ifndef EVENT_H
#define EVENT_H
#include "../types.h"
#include <functional>
#include <string>
namespace Lina{ namespace Events{
    enum class Type : u16
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowMove,
        AppTick, AppUpdate, AppRender,
        KeyDown, KeyUp,
        MouseDown, MouseUp, MouseMove, MouseScroll
    };

    class Event
    {
        public:
            virtual ~Event() = default;
            virtual Type getType() const {return Type::None;};
            //virtual std::string getName() const = 0;
            template<typename ...Args, typename ...fArgs>
            void subscribe(std::function<void(Args...)>f, fArgs&&... args) 
            {
                std::function<void()> f2 = std::bind(f, args...);
                subscribers.push_back(f2);
            }
            u32 remainingSubs() const {return subscribers.size();}
            b8 handle()
            {
                for (auto f : subscribers)
                {
                    f();
                }
                handled = true;
                subscribers.clear();
                return true;
            }
        protected:
            bool handled = false;
            std::vector<std::function<void()>> subscribers;
    };
}}
#endif

