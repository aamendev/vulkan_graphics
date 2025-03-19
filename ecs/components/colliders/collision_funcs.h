#ifndef COLLISION_FUNCS
#define COLLISION_FUNCS
#include "collider.h"
namespace Lina { namespace Helpers { namespace Collisions{
#define COLLISION_EPSILON (0.0001f)

        typedef struct Simplex
        {
            u32 size = 0;
            std::vector<Math::Vector3D> points;

            inline void push(Math::Vector3D& v) {points.push_back(v); size++;}
            inline void push_front(Math::Vector3D& v) 
            {points.insert(points.begin(), v); size++;}
            inline void pop(Math::Vector3D& v) {points.pop_back(); size--;}
            inline Math::Vector3D& at(u32 idx) {return points[idx];}
            inline auto begin() {return points.begin();}
            inline auto end() {return points.end();}
            
            void set(std::vector<Math::Vector3D>&& v) 
            {
                size = v.size();
                points.clear();
                std::move(v.begin(), v.end(), std::back_inserter(points));
            }
            void clear()
            {size = 0; points.clear();}
        } Simplex;
    using namespace ECS::Components;

        typedef struct Info
        {
            f32 depth = 0.0f;
            Math::Vector3D normal = {0, 0, 0};
            b8 collided = false;
        } Info;

    Math::Vector3D support(Collider* a, Collider* b, const Math::Vector3D& d);
    b8 gjk(Collider* a, Collider* b);
    Info gjkAndInfo(Collider* a, Collider* b);
    b8 checkNextSimplex(Simplex&, Math::Vector3D& d);
    b8 checkLine(Simplex&, Math::Vector3D& d);
    b8 checkTri(Simplex&, Math::Vector3D& d);
    b8 checkTetra(Simplex&, Math::Vector3D& d);

    Info epa(Simplex& s, Collider* a, Collider* b);

    std::pair<std::vector<Math::Vector4D>, u32> normalAndMin(
            std::vector<Math::Vector3D>& verts,
            std::vector<u32>& indices);
    void addUniqueRemoveDup(std::vector<std::pair<u32, u32>>& edges, std::vector<u32>& faces,
            u32 a, u32 b);
    
}}}
#endif
