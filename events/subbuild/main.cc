#include <iostream>
#include "../mouse.h"
#include "../../Types.h"
#include "../key.h"

b8 printMouse(Lina::Events::MouseMove& e)
{

    std::cout<< "Mouse Moved: (" << e.x() << ", " << e.y() << ")\n"; return true; }

b8 getKey(Lina::Events::KeyPress& e)
{
    b8 hold = e.isHold();
    auto k = e.key();
    if (hold)
    {
        std::cout << "Key is held: " << (u32)k << '\n';
    }
    else 
    {
        std::cout<< (u32)k << '\n';
    }
    return true;
}

b8 moveDrone(Lina::Events::KeyPress& e)
{
    auto k = e.key();
    if (k == Input::KeyCode::W || k == Input::KeyCode::Up)
    {
        std::cerr<<"FORWARD\n";
    }

    if (k == Input::KeyCode::S || k == Input::KeyCode::Down)
    {
        std::cerr<<"DOWNWARD\n";
    }

    return true;
}
class wind
{
    public:
        friend class app;
        u32 width; u32 height;
        std::vector<Lina::Events::Event*> windowEvents;
        void mousemove(f32 x, f32 y) {
            Lina::Events::MouseMove* me = new Lina::Events::MouseMove(x, y); 
            windowEvents.push_back(dynamic_cast<Lina::Events::Event*>(me));}

        void inputKey(Input::KeyCode k, b8 hold){
            Lina::Events::KeyPress* ke = new Lina::Events::KeyPress(k, hold); 
            windowEvents.push_back(dynamic_cast<Lina::Events::Event*>(ke));}

        std::vector<Lina::Events::Event*> getEvents() {return windowEvents;}
};
class app
{
    public:
        app(wind* aw) : w(aw), es({}){}
        void run();
        template <typename EventType, typename Callback>  
            void catSub(Callback f) 
            {
                for (int i = 0; i < es.size(); i++)
                {
                    auto* e = es[i];
                    if (e->getType() == EventType::getStaticType())
                    {
                        e->subscribe(f, static_cast<EventType&>(*e));
                    }
                }
                //es.size();
            };
        void getWinEvents() {
            es = w->getEvents();
        }
        void handleEvents()
        {
            for (auto* e : es)
            {
                e->handle();
                delete e;
            }
        }
    private:
        wind* w;
        std::vector<Lina::Events::Event*> es;
};


int main()
{
    wind w;
    app ma(&w);

    w.mousemove(20, 30);
    w.mousemove(10, 5);
    w.mousemove(66, 23.5);
    w.inputKey(Input::KeyCode::A, false);
    w.inputKey(Input::KeyCode::W, false);
    w.inputKey(Input::KeyCode::W, true);
    w.getEvents();
     ma.getWinEvents();
       ma.catSub<Lina::Events::MouseMove>(std::function(printMouse));
       ma.catSub<Lina::Events::KeyPress>(std::function(moveDrone));
       ma.catSub<Lina::Events::KeyPress>(std::function(getKey));
       ma.handleEvents();
       
}
