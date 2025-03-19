#include "validation_layer.h"
#include <cassert>
#include <iostream>
#include <memory>

namespace Lina { namespace Graphics {
    void ValidationLayer::init()
    {
        std::cerr << "Begin Collision Validation : \n\n";
        collisionValidation();
        std::cerr << "\n\nEnd Collision Validation \n\n";

        std::cerr << "Begin Transform Validation : \n\n";
        transformValidation();
        std::cerr << "\n\nEnd Transform Validation : \n\n";

        std::cerr << "Begin Health Validation : \n\n";
        healthValidation();
        std::cerr << "\n\nEnd Health Validation : \n\n";

        std::cerr << "Begin Timer Validation : \n\n";
        timerValidation();
        std::cerr << "\n\nEnd Timer Validation : \n\n";

        std::cerr << "Begin Physics Validation : \n\n";
        physicsValidation();
        std::cerr << "\n\nEnd Physics Validation : \n\n";

    }

    void ValidationLayer::physicsValidation()
    {
        particleValidation();
    }

    void ValidationLayer::particleValidation()
    {
        p = new Physics::Particle();
        p->setMass(5);
        float timeStep = 0.001;
        Math::Vector3D init = {0, 0, 0};
        for (int i = 0; i < 10; i++)
        {
            p->update(i * timeStep);
            assert(p->getVelocity() == init);
            assert(p->getAcc() == init);
            assert(p->getPos() == init);
        }

        p->setVelocity({20, 5, 0});

        Math::Vector3D acc = {0, -10, 0};

        for (int i = 0; i < 1000; i++)
        {
            p->applyImpulse({0, -50, 0});
            p->update(timeStep);
        }
        assert(p->getAcc() == init);

        p->applyForce({0, -50, 0});
        for (int i = 0; i < 1000; i++)
        {
            p->update(timeStep);
        }
        assert(p->getAcc() == acc);

        p->setVelocity({20, 5, 0});
        p->setPosition({0, 0 ,0});

        p->applyForce({0, 50, 0});
        p->applyForce({0, -10, 0});
        p->applyForce({0, -30, 0});
        p->applyForce({0, -7, 0});
        p->applyForce({0, -3, 0});

        for (int i = 0; i < 1000; i++)
        {
            p->update(timeStep);
        }
        assert(p->getAcc() == acc);

        std::cerr << p->getVelocity() << ":Vel\n"; 
        std::cerr << p->getPos() << ":Pos\n"; 
        delete p;
    }
    void ValidationLayer::timerValidation()
    {
        mTimer.begin();
        mTimer.wait(100);
        mTimer.end();
        std::cerr << mTimer.getTime() << '\n';
        mTimer.wait(100);
        std::cerr <<mTimer.getTime();
    }

    void ValidationLayer::healthValidation()
    { 
        auto& h = health1.getHealth();

        assert(h == 100);

        health1.instantDeath();

        assert(h == 0);

        health1.heal(34);

        assert(h == 34);

        health1.heal(24);

        assert(h == 58);

        health1.takeDamage(20);

        assert(h == 38);

        health1.restore();

        assert(h == 100);
    }

    void ValidationLayer::transformValidation()
    {
        trans1.setPosition({2, 3, 4});
        trans1.setRotation({180 * PI / 180, 0, 0});

        Math::Quatrenion result = {1, 0, 0, 0};
        assert(trans1.getRotation() == result);
        std::cerr << "QUAT: " << trans1.getRotation() << "\n";

        trans1.translate({1, 0, 10});
        const Math::Point3D newpos = {3, 3, 14};
        assert(trans1.getPosition() == newpos);


        
        trans1.rotate({PI, 0, 0});
        trans1.rotate({0, 20 * PI / 180, 90 * PI / 180});
        auto rr = trans1.getRotationAngles();
        std::cerr << trans1.getRotation() << "\n";

        std::cerr << trans1.getMatrix() << "\n";
    }


    void alternativeOnCollision(ECS::Components::Collider* c1, ECS::Components::Collider* c2)
    {
        std::cerr <<"!!!Alternative VERSION!!!!\n";
    }

    void ValidationLayer::collisionValidation()
    {
        c1 = new ECS::Components::Colliders::Cylinder("c1");
        c2 = new ECS::Components::Colliders::Cylinder("c2");
        p1 = new ECS::Components::Colliders::Plane("p1");

        cSystem.addCollider(c1);
        cSystem.addCollider(c2);
        cSystem.addCollider(p1);

        c1->setHeight(10);
        c1->setRadius(5);
        c1->setPosition({0, 0, 0});

        c2->setHeight(10);
        c2->setRadius(5);
        c2->setPosition({1, 0, 0});

        assert(c1->checkCollision((ECS::Components::Collider*)c2) == true);
        cSystem.update();
        assert(c1->checkCollision((ECS::Components::Collider*)c2) == false);

        c2->setPosition({-2, 1, 2});
        assert(c1->checkCollision((ECS::Components::Collider*)c2) == true);
        cSystem.update();
        assert(c1->checkCollision((ECS::Components::Collider*)c2) == false);
        
        c2->setPosition({100, 100, 100});
        c1->setPosition({0, 0, 0});

        std::cerr << "\n***************\n";
        assert(c1->checkCollision(p1) == true);
        cSystem.update();
        std::cerr << "C1: " << c1->getCenter() <<", P1:"<< p1->getCenter() << "\n";
        assert(c1->checkCollision(p1) == false);
        c1->setPosition({-2, 5, 0});
        assert(c1->checkCollision(p1) == false);
        cSystem.update();

        c1->setPosition({-2, 3, 0});
        assert(c1->checkCollision(p1) == true);
        cSystem.update();
        assert(c1->checkCollision(p1) == false);

        c1->setPosition({-2, 10, 0});
        assert(c1->checkCollision(p1) == false);


        std::cerr << "\n***************\n";

        c1->setOnCollisionEnter(alternativeOnCollision);
        c1->setCallDefaults(false);
        c1->setPosition({-2, 3, 0});
        assert(c1->checkCollision(p1) == true);
        cSystem.update();
        assert(c1->checkCollision(p1) == true);
        delete c1;
        delete c2;
        delete p1;
    }

}}
