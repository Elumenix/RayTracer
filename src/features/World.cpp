#include "World.h"

World World::Default()
{
    World w = World();

    Light light = Light(Point(-10,10,-10), Color(1,1,1));
    w.add(light);
    
    Sphere s1;
    s1.material.color = Color(0.8,1.0,0.6);
    s1.material.diffuse = 0.7;
    s1.material.specular = 0.2;
    w.add(std::move(s1));

    Sphere s2;
    s2.transform = transformations::scaling(0.5,0.5,0.5);
    w.add(std::move(s2));
    
    return w;
}
